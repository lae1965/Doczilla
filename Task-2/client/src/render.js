import { deleteStudentById } from "./api.js";

const parentOfListing = document.querySelector('.listing__students');

const createNewElement = (isToEnd, parent, teg, classNames, text) => {
  const element = document.createElement(teg);
  classNames?.forEach((className) => {
    element.classList.add(className);
  })
  if (text) element.innerText = text;
  if (isToEnd) parent.append(element);
  else parent.prepend(element);
  return element;
}

export const createNewStudent = (student) => {
  const row = createNewElement(false, parentOfListing, 'ul', ['table-line', 'listing__item']);
  row.setAttribute('tabindex', '0');

  createNewElement(true, row, 'li', ['id', 'item'], student.id);
  createNewElement(true, row, 'li', ['item'], student.lastName);
  createNewElement(true, row, 'li', ['item'], student.firstName);
  createNewElement(true, row, 'li', ['item'], student.patronymic);
  createNewElement(true, row, 'li', ['item'], student.birthDate);
  createNewElement(true, row, 'li', ['item'], student.group);

  const li = createNewElement(true, row, 'li', ['erase']);
  const button = createNewElement(true, li, 'button');
  button.setAttribute('type', 'button');
  button.setAttribute('title', 'Удалить');
  button.setAttribute('tabindex', '-1');
  const img = createNewElement(true, button, 'img');
  img.setAttribute('src', './public/erase.png');
  img.setAttribute('alt', 'erase');

  button.addEventListener('click', async (e) => {
    try {
      let parent = e.target;
      do {
        parent = parent.parentNode;
      } while (!parent.classList.contains('listing__item'));
      const id = parent.querySelector('.id').innerText;
      await deleteStudentById(id);
      removeElement(parent);
    } catch (e) {
      alert(e);
      return;
    }
  });
}

export const renderListOfStudents = (studentsList) => {
  studentsList.forEach((student) => {
    createNewStudent(student);
  });
}

export const removeElement = (element) => {
  element.innerHTML = '';
  element.remove();
}