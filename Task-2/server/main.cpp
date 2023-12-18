#include <cstdlib>
#include <cstdint>
#include <cerrno>
#include <cstring>
#include <cstdio>

#include <array>
#include <map>
#include <string>
#include <string_view>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>

#include <nlohmann/json.hpp>
#include <sqlite3.h>

static sqlite3* db;

struct request_holder {
    std::array<char, 8196> read_buffer;
    size_t read_buffer_size;
    size_t header_size;
    std::map<std::string, std::string> header;
};

struct request {
    const std::map<std::string, std::string>& header;
    std::string_view body;
};

struct response {
    size_t code;
    std::string body;
    std::map<std::string, std::string> headers;
};

int create_listener(uint16_t port) {
    int reuse_addr = true;
    sockaddr_in6 addr{};
    int fd;

    // Create socket
    fd = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
        printf("%s:%d socket: %s\n", __FILE__, __LINE__ - 2, strerror(errno));
        return -1;
    }

    // Enable reuse address
    if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof reuse_addr) == -1) {
        printf("%s:%d setsockopt: %s\n", __FILE__, __LINE__ - 2, strerror(errno));
        close(fd);
        return -1;
    }

    // Bind address
    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_any;
    addr.sin6_port = htons(port);
    if (::bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof addr) == -1) {
        printf("%s:%d bind: %s\n", __FILE__, __LINE__ - 2, strerror(errno));
        close(fd);
        return -1;
    }

    // Start listening
    if (::listen(fd, SOMAXCONN) == -1) {
        printf("%s:%d listen: %s\n", __FILE__, __LINE__ - 2, strerror(errno));
        close(fd);
        return -1;
    }

    return fd;
}

bool read_header(int client, request_holder& request) {
    auto& read_buffer = request.read_buffer;
    auto& read_buffer_size = request.read_buffer_size;
    auto& header_size = request.header_size;

    bool header_end = false;
    int ret;
    size_t bytes_read = 0;
    size_t bytes_left = read_buffer.size();

    while(!header_end && bytes_left > 0) {
        ret = ::recv(client, read_buffer.data() + bytes_read, bytes_left, 0);
        if (ret == -1) {
            // Connection closed
            printf("Connection closed\n");
            return false;
        }

        bytes_read += ret;
        bytes_left -= ret;

        for (size_t i = 0; i < bytes_read - 3; ++i) {
            if (strncmp("\r\n\r\n", read_buffer.data() + i, 4) == 0) {
                header_size = i + 4;
                header_end = true;
                break;
            }
        }
    }
    read_buffer_size = bytes_read;

    return header_end;
}

bool parse_header(request_holder& request) {
    auto& read_buffer = request.read_buffer;
    auto& header_size = request.header_size;
    auto& header = request.header;

    bool method = false;
    bool uri = false;
    bool version = false;

    bool key = false;
    size_t key_start = 0;
    size_t key_end = 0;
    size_t value_start = 0;
    size_t value_end = 0;

    for (size_t i = 0; i < header_size && strncmp("\r\n\r\n", read_buffer.data() + i, 4) != 0; ++i) {
        if (!method) {
            if (read_buffer[i] == ' ') {
                header["method"] = std::string(read_buffer.data() + value_start, value_end - value_start);
                method = true;
                value_end += 1;
                value_start = value_end;
            } else {
                ++value_end;
            }
        } else if (!uri) {
            if (read_buffer[i] == ' ') {
                header["request_uri"] = std::string(read_buffer.data() + value_start, value_end - value_start);
                uri = true;
                value_end += 1;
                value_start = value_end;
            } else {
                ++value_end;
            }
        } else if (!version) {
            if (read_buffer[i] == '\r') {
                header["version"] = std::string(read_buffer.data() + value_start, value_end - value_start);
                version = true;
                ++i;
                key_start = value_end + 2;
                key_end = key_start;
            } else {
                ++value_end;
            }
        } else {
            if (!key) {
                if (read_buffer[i] == ':') {
                    key = true;
                    ++i;
                    value_start = key_end + 2;
                    value_end = value_start;
                } else {
                    ++key_end;
                }
            } else {
                if (read_buffer[i] == '\r') {
                    header[std::string(read_buffer.data() + key_start, key_end - key_start)] =
                        std::string(read_buffer.data() + value_start, value_end - value_start);
                    ++i;
                    key_start = value_end + 2;
                    key_end = key_start;
                    key = false;
                } else {
                    ++value_end;
                }
            }
        }
    }
    header[std::string(read_buffer.data() + key_start, key_end - key_start)] =
        std::string(read_buffer.data() + value_start, value_end - value_start);

    return true;
}

bool read_body(int client, request_holder& request) {
    auto& read_buffer = request.read_buffer;
    auto& read_buffer_size = request.read_buffer_size;
    auto& header = request.header;
    auto& header_size = request.header_size;

    size_t bytes_read;
    size_t bytes_left;
    int ret;

    // Check Content-Length
    auto content_length = header.find("Content-Length");
    if (content_length == header.end()) {
        // No body
        return true;
    }

    // Read body
    bytes_read = read_buffer_size;
    bytes_left = header_size + std::stoi(content_length->second) - read_buffer_size;
    while(bytes_left > 0) {
        ret = ::recv(client, read_buffer.data() + bytes_read, bytes_left, 0);
        if (ret == -1) {
            // Connection closed
            return false;
        }

        bytes_read += ret;
        bytes_left -= ret;
    }

    return true;
}

response add_student(const request& request) {
    // Check content-type
    // auto content_type_it = request.header.find("Content-Type");
    // if (content_type_it == request.header.end() || content_type_it->second != "application/json") {
    //     return response{ .code = 400 };
    // }

    // Parse JSON
    nlohmann::json body = nlohmann::json::parse(request.body);
    
    // Check JSON content
    if (!body.contains("lastName") || !body.contains("firstName") ||
        !body.contains("patronymic") || !body.contains("birthDate") ||
        !body.contains("group")) {
        return response{ .code = 400 };
    }

    // Add new student
    std::string sql_statement = 
        "INSERT INTO `students` "
        "(`lastName`, `firstName`, `patronymic`, `birthDate`, `group`) "
        "VALUES('" + body["lastName"].get<std::string>() + "', '" + body["firstName"].get<std::string>() + "', '" + body["patronymic"].get<std::string>() + "', '" + body["birthDate"].get<std::string>() + "', '" + body["group"].get<std::string>() + "') "
        "RETURNING `id`;";

    sqlite3_stmt* sql;
    if (sqlite3_prepare(db, sql_statement.c_str(), sql_statement.size(), &sql, nullptr) != SQLITE_OK) {
        return response{ .code = 500 };
    }
    if (sqlite3_step(sql) != SQLITE_ROW) {
        return response{ .code = 500 };
    }

    // Get new id
    int new_id = sqlite3_column_int(sql, 0);

    sqlite3_finalize(sql);

    // Return new student
    body["id"] = new_id;

    return response{ .code = 201, .body = body.dump(), .headers = { { "Content-Type", "application/json" } } };
}

response delete_student(const request& request) {
    auto& request_uri = request.header.find("request_uri")->second;

    // Get id
    size_t id = std::stoi(request_uri.substr(5));

    // Delete student
    std::string sql_statement = 
        "DELETE FROM `students` WHERE `id` = " + std::to_string(id) + ";";

    sqlite3_stmt* sql;
    if (sqlite3_prepare(db, sql_statement.c_str(), sql_statement.size(), &sql, nullptr) != SQLITE_OK) {
        return response{ .code = 500 };
    }
    if (sqlite3_step(sql) != SQLITE_DONE) {
        return response{ .code = 500 };
    }
    sqlite3_finalize(sql);

    return response{ .code = 204 };
}

response get_students(const request& request) {
    nlohmann::json students = nlohmann::json::array();
    size_t i = 0;

    // Get students
    std::string sql_statement = "SELECT * FROM `students`;";

    sqlite3_stmt* sql;
    if (sqlite3_prepare(db, sql_statement.c_str(), sql_statement.size(), &sql, nullptr) != SQLITE_OK) {
        return response{ .code = 500 };
    }
    while (sqlite3_step(sql) != SQLITE_DONE) {
        students[i]["id"] = sqlite3_column_int(sql, 0);
        students[i]["lastName"] = reinterpret_cast<const char*>(sqlite3_column_text(sql, 1));
        students[i]["firstName"] = reinterpret_cast<const char*>(sqlite3_column_text(sql, 2));
        students[i]["patronymic"] = reinterpret_cast<const char*>(sqlite3_column_text(sql, 3));
        students[i]["birthDate"] = reinterpret_cast<const char*>(sqlite3_column_text(sql, 4));
        students[i]["group"] = reinterpret_cast<const char*>(sqlite3_column_text(sql, 5));
        ++i;
    }
    sqlite3_finalize(sql);

    return response{ .code = 200, .body = students.dump(), .headers = { { "Content-Type", "application/json" } } };
}

response options(const request& request) {
    return response{ .code = 200, .headers = { { "Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE" }, { "Access-Control-Allow-Headers", "Content-Type" } } };
}

response handle_request(const request& request) {
    auto& method = request.header.find("method")->second;
    auto& request_uri = request.header.find("request_uri")->second;

    if (method == "POST" && request_uri == "/api") {
        return add_student(request);
    } else if (method == "DELETE" && strncmp("/api", request_uri.c_str(), 4) == 0) {
        return delete_student(request);
    } else if (method == "GET" && request_uri == "/api") {
        return get_students(request);
    } else if (method == "OPTIONS") {
        return options(request);
    }

    return response{ 404 };
}

void send_response(int client, const response& response) {
    // Common header
    std::string response_buffer = 
        "HTTP/1.1 " + std::to_string(response.code) + "\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Connection: close\r\n";
    
    // Additional headers
    if (response.body.size() > 0) {
        response_buffer += "Content-Length: " + std::to_string(response.body.size()) + "\r\n";
    }
    for (auto& [key, value] : response.headers) {
        response_buffer += key + ": " + value + "\r\n";
    }
    response_buffer += "\r\n";
    response_buffer += response.body;

    size_t bytes_sent = 0;
    size_t bytes_left = response_buffer.size();
    int ret;

    while (bytes_left > 0) {
        ret = ::send(client, response_buffer.data() + bytes_sent, bytes_left, 0);
        if (ret == -1) {
            // Connection closed
            return;
        }

        bytes_sent += ret;
        bytes_left -= ret;
    }
}

bool accept_n_handle(int listener) {
    sockaddr_in6 new_addr;
    socklen_t new_addr_len = sizeof new_addr;
    char new_addr_str[INET6_ADDRSTRLEN];
    int new_fd;

    request_holder request{};

    // Accept new connection
    new_fd = ::accept(listener, reinterpret_cast<sockaddr*>(&new_addr), &new_addr_len);
    if (new_fd == -1) {
        if (errno == ENETDOWN || errno == EPROTO || errno == ENOPROTOOPT ||
            errno == EHOSTDOWN || errno == ENONET || errno == EHOSTUNREACH ||
            errno == EOPNOTSUPP || errno == ENETUNREACH) {
            // Client/Network side error
            return true;
        } else {
            // Server-side error
            return false;
        }
    }

    printf("New connection from %s\n", inet_ntop(AF_INET6, &new_addr.sin6_addr, new_addr_str, sizeof new_addr_str));
    
    if (read_header(new_fd, request) && parse_header(request) && read_body(new_fd, request)) {
        send_response(new_fd, handle_request({
            .header = request.header,
            .body = std::string_view(request.read_buffer.data() +request.header_size)
        }));
    }

    close(new_fd);
    return true;
}

int main(int argc, char** argv) {
    // Parse args
    const char* db_path = "students.db";
    uint16_t port = 8080;

    int ret;
    while ((ret = getopt(argc, argv, "p:d:")) != -1) {
        switch(ret) {
        case 'p':
            port = std::stoi(optarg);
            break;
        case 'd':
            db_path = optarg;
            break;
        case '?':
            printf("Unknown argument\n");
            return EXIT_FAILURE;
        }
    }

    // Connect to database
    if (sqlite3_open(db_path, &db) != SQLITE_OK) {
        printf("%s:%d sqlite3_open: %s\n", __FILE__, __LINE__ - 1, sqlite3_errmsg(db));
        return EXIT_FAILURE;
    }

    // Initialize database
    if (sqlite3_exec(db, 
        "CREATE TABLE IF NOT EXISTS `students` (`id` INTEGER PRIMARY KEY AUTOINCREMENT, `lastName` TEXT, `firstName` TEXT, `patronymic` TEXT, `birthDate` TEXT, `group` TEXT);", nullptr, nullptr, nullptr) != SQLITE_OK) {
        printf("%s:%d sqlite3_exec: %s\n", __FILE__, __LINE__ - 1, sqlite3_errmsg(db));
        return EXIT_FAILURE;
    }

    int listener = create_listener(port);
    if (listener == -1) {
        printf("Failed to create listener\n");
        return EXIT_FAILURE;
    }

    while(accept_n_handle(listener));

    close(listener);
    return EXIT_SUCCESS;
}