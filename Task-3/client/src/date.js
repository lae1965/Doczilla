import { curDate, setCurDate, weekDates } from "./index.js";

export const getDateToday = () => {
  return new Date();
}

export const getShortDate = (date) =>
  date.toLocaleDateString(
    'ru-RU', {
    day: 'numeric',
    month: 'short',
    year: 'numeric'
  }).slice(0, -3);

export const getMonthAndYear = (date) =>
  getShortDate(date).split(' ').slice(1).join(' ');

export const parseDate = (date) => {
  const day = date.getDate();
  const month = date.getMonth() + 1;
  const year = date.getFullYear();
  let firstWeekDayOfMonth = new Date(`${year}-${month}-01`).getDay();
  if (firstWeekDayOfMonth === 0) firstWeekDayOfMonth = 7;
  const lastDayOfMonth = new Date(year, month, 0).getDate();

  return {
    day, month, year, firstWeekDayOfMonth, lastDayOfMonth
  };
}

export const getNewMonth = (back) => {
  let { day, month, year } = parseDate(curDate);

  if (back) {
    if (month === 1) {
      month = 12;
      year--;
    } else month--;
  } else {
    if (month === 12) {
      month = 1;
      year++;
    } else month++;
  }

  const lastDayOfMonth = new Date(year, month, 0).getDate();

  if (lastDayOfMonth < day) day = lastDayOfMonth;

  setCurDate(new Date(year, month - 1, day));
  return getMonthAndYear(curDate);
}

export const setNewDay = (newDay) => {
  const { month, year } = parseDate(curDate);
  setCurDate(new Date(year, month - 1, newDay));
}

export const setCurWeek = () => {
  const { day } = parseDate(curDate);
  const dayOfWeek = curDate.getDay() || 7;
  let date = new Date(curDate);
  weekDates.endOfWeek = new Date(date.setDate(day - dayOfWeek + 7));
  date = new Date(curDate);
  weekDates.startOfWeek = new Date(date.setDate(day - dayOfWeek + 1));
}