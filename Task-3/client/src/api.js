const URL = 'http://localhost:3333/api';

export const getAllTasksListing = async () => {
  try {
    const response = await fetch(URL);
    if (!response.ok) throw new Error('Something wrong!!!');
    const data = await response.json();
    return await data;
  } catch (e) {
    throw e;
  }
}

export const getTasksListingByQueryString = async (searchString) => {
  try {
    const response = await fetch(`${URL}/find/${searchString}`);
    if (!response.ok) throw new Error('Something wrong!!!');
    return await response.json();
  } catch (e) {
    throw e;
  }
}

export const getTasksListingByDateRangeAndStatus = async ({ from, to, status }) => {
  try {
    let url = `${URL}/date?from=${from}&to=${to ? to : from}`;
    if (status !== undefined) url += `&status=${status}`;
    const response = await fetch(url);
    if (!response.ok) throw new Error('Something wrong!!!');
    return await response.json();
  } catch (e) {
    throw e;
  }
}
