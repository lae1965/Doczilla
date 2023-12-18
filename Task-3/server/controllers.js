class Controller {
  url;
  constructor() {
    this.url = 'https://todo.doczilla.pro/api/todos';
  }

  async getAllTasks(req, res) {
    try {
      const response = await fetch(this.url);
      if (!response.ok) throw new Error('Something wrong!!!');
      return res.json(await response.json());
    } catch (e) {
      throw e;
    }
  }

  async getTasksBySearchString(req, res) {
    try {
      const response = await fetch(`${this.url}/find?q=${req.params.searchString}`);
      if (!response.ok) throw new Error('Something wrong!!!');
      return res.json(await response.json());
    } catch (e) {
      throw e;
    }
  }

  async getTasksByDateAndStatus(req, res) {
    try {
      const { from, to, status } = req.query;
      let url = `${this.url}/date?from=${from}&to=${to}`;
      if (status !== undefined) url += `&status=${status}`;

      const response = await fetch(url);
      if (!response.ok) throw new Error('Something wrong!!!');
      return res.json(await response.json());
    } catch (e) {
      throw e;
    }
  }
}

export default new Controller();