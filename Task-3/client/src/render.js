import { checkboxSVG } from './checkboxSVG.js';
import { getMonthAndYear, getShortDate, parseDate, setNewDay } from './date.js';
import { curDate, date, days, monthYear, putDateToSearch, requestToApi } from './index.js';

const main = document.querySelector('.main');
const parentTask = document.querySelector('.tasks-listing')
const modal = document.querySelector('.modal')

const createNewElement = (parent, teg, className, text) => {
  const element = document.createElement(teg);
  if (className) element.classList.add(className);
  if (text) element.innerText = text;
  parent.append(element);
  return element;
}

export const createListOfTasks = (tasksList) => {
  parentTask.innerHTML = '';
  tasksList.forEach(taskItem => {
    const task = createNewElement(parentTask, 'div', 'task');
    const taskDescription = createNewElement(task, 'div', 'task__description');
    const taskContent = createNewElement(taskDescription, 'div', 'task__content');
    createNewElement(taskContent, 'h3', 'task__title', taskItem.name);
    createNewElement(taskContent, 'p', 'task__text', taskItem.shortDesc);
    const checkbox = createNewElement(taskDescription, 'div', 'checkbox');
    checkbox.innerHTML = checkboxSVG;

    createNewElement(task, 'p', 'task__date', taskItem.date.slice(0, -12).split('T').join(' '));

    if (taskItem.status) {
      const check = checkbox.querySelector('#check-mark');
      check.classList.remove('uncheck');
    }

    task.addEventListener('click', (e) => {
      modal.classList.add('modal__shown');
      const modalName = modal.querySelector('.modal__title');
      modalName.innerText = taskItem.name;
      const modalDate = modal.querySelector('.modal__date');
      modalDate.innerText = taskItem.date.slice(0, -12).split('T').join(' ');
      if (taskItem.status) {
        const check = modal.querySelector('.mark');
        check.classList.remove('uncheck');
      }
      const modalText = modal.querySelector('.modal__text');
      modalText.innerText = taskItem.fullDesc;
      const modalReady = modal.querySelector('.modal__ready');
      modalReady.addEventListener('click', () => {
        modal.classList.remove('modal__shown');
      });
    });

  });
}

export const createCalendar = (newDate) => {
  days.innerHTML = '';
  const { day, month, year, firstWeekDayOfMonth, lastDayOfMonth } = parseDate(newDate);

  monthYear.innerText = getMonthAndYear(newDate);
  for (let i = 0; i < lastDayOfMonth; i++) {
    const d = createNewElement(days, 'div', 'day', (i + 1).toString());
    if (i === 0) d.style.gridColumn = firstWeekDayOfMonth;
    if (day === i + 1) d.classList.add('bg-yellow');
    d.addEventListener('click', async (e) => {
      try {
        setNewDay(e.target.innerText);
        putDateToSearch();
        createCalendar(curDate);
        await requestToApi();
      } catch (e) {
        throw e;
      }
    });
  }
}