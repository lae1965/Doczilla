const URL = 'http://192.168.0.2:8080/api';

export const saveNewStudent = async (data) => {
  console.log(data);
  console.log(JSON.stringify(data));
  try {
    const response = await fetch(URL, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(data)
    });
    if (!response.ok) throw new Error("Something wrong!!! Can't create student");
    return await response.json();
  } catch (e) {
    alert(e);
    return;
  }
}

export const deleteStudentById = async (id) => {
  try {
    const response = await fetch(`${URL}/${id}`, {
      method: 'DELETE'
    });
    if (!response.ok) throw new Error("Something wrong!!! Can't delete student");
    // await response.json();
  } catch (e) {
    alert(e);
    return;
  }
}

export const getListOfAllStudents = async () => {
  try {
    const response = await fetch(URL);
    if (!response.ok) throw new Error("Something wrong!!! Can't get list of all students.");
    return await response.json();
  } catch (e) {
    alert(e);
    return;
  }
}