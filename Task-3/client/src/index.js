import { getAllTasksListing, getTasksListingByDateRangeAndStatus, getTasksListingByQueryString } from './api.js';
import { getDateToday, getNewMonth, getShortDate, setCurWeek } from './date.js';
import { createCalendar, createListOfTasks } from './render.js';

const searchParamsDate = {
  ALL: 0,
  DAY: 1,
  WEEK: 2
};

let searchParam = searchParamsDate.ALL;
let response;
let sortForward = true;

const form = document.querySelector('.search');
const searchInput = document.querySelector('.search__input');
const forDay = document.querySelector('.today');
const forWeek = document.querySelector('.for-week');
const monthLeft = document.querySelector('.month__left');
const monthRight = document.querySelector('.month__right');
export const monthYear = document.querySelector('.month__year');
export const days = document.querySelector('.days');
const onlyNotDone = document.querySelector('.only-not-done');
const isOnlyNotDone = document.querySelector('#checkbox');
export const date = document.querySelector('.date');
const sort = document.querySelector('.sort');

export let curDate = getDateToday();
export const setCurDate = (newDate) => { curDate = newDate; }

export const weekDates = {};

createCalendar(curDate);

export const putDateToSearch = () => {
  switch (searchParam) {
    case searchParamsDate.ALL:
      date.innerText = '';
      isOnlyNotDone.checked = false;
      break;
    case searchParamsDate.DAY:
      date.innerText = getShortDate(curDate);
      break;
    case searchParamsDate.WEEK:
      setCurWeek();
      date.innerText = `${getShortDate(weekDates.startOfWeek)} - ${getShortDate(weekDates.endOfWeek)}`;
      break;
  }
}

export const requestToApi = async () => {
  try {
    if (!!searchInput.value) {
      searchParam = searchParamsDate.ALL;
      putDateToSearch();
      response = await getTasksListingByQueryString(searchInput.value)
    } else {
      if (searchParam === searchParamsDate.ALL) response = await getAllTasksListing();
      else {
        const requestParams = {};
        if (isOnlyNotDone.checked) requestParams.status = false;
        if (searchParam === searchParamsDate.DAY) requestParams.from = Date.parse(curDate);
        else {
          requestParams.from = Date.parse(weekDates.startOfWeek);
          requestParams.to = Date.parse(weekDates.endOfWeek);
        }
        response = await getTasksListingByDateRangeAndStatus(requestParams);
      }
    }
    createListOfTasks(response);
  } catch (e) {
    throw e;
  }
}

(async () => {
  try {
    await requestToApi();
  } catch (e) {
    alert(e);
    return;
  }
})();

form.addEventListener('submit', async (e) => {
  try {
    e.preventDefault();
    await requestToApi();
    searchInput.value = '';
  } catch (e) {
    throw e;
  }
});

monthLeft.addEventListener('click', async () => {
  try {
    monthYear.innerText = getNewMonth(true);
    putDateToSearch();
    createCalendar(curDate);
    await requestToApi();
  } catch (e) {
    throw e;
  }
});

monthRight.addEventListener('click', async () => {
  try {
    monthYear.innerText = getNewMonth(false);
    putDateToSearch();
    createCalendar(curDate);
    await requestToApi();
  } catch (e) {
    return e;
  }
});

forDay.addEventListener('click', async () => {
  try {
    if (searchParam === searchParamsDate.WEEK) forWeek.classList.remove('bg-yellow');
    if (searchParam === searchParamsDate.DAY) {
      searchParam = searchParamsDate.ALL;
      forDay.classList.remove('bg-yellow');
    } else {
      searchParam = searchParamsDate.DAY;
      forDay.classList.add('bg-yellow');
    }
    putDateToSearch();
    await requestToApi()
  } catch (e) {
    throw e;
  }
});

forWeek.addEventListener('click', async () => {
  try {
    if (searchParam === searchParamsDate.DAY) forDay.classList.remove('bg-yellow');
    if (searchParam === searchParamsDate.WEEK) {
      searchParam = searchParamsDate.ALL;
      forWeek.classList.remove('bg-yellow');
    } else {
      searchParam = searchParamsDate.WEEK;
      forWeek.classList.add('bg-yellow');
    }
    putDateToSearch();
    await requestToApi();
  } catch (e) {
    throw e;
  }
});

onlyNotDone.addEventListener('click', async () => {
  try {
    if (searchParam === searchParamsDate.ALL || !!searchInput.value) {
      isOnlyNotDone.checked = false;
      return;
    }
    await requestToApi();
  } catch (e) {
    throw e;
  }
});

sort.addEventListener('click', () => {
  if (sortForward) response.sort((a, b) => Date.parse(a.date) - Date.parse(b.date));
  else response.sort((a, b) => Date.parse(b.date) - Date.parse(a.date));
  sortForward = !sortForward;
  createListOfTasks(response);
});