import { deleteStudentById, getListOfAllStudents, saveNewStudent } from "./api.js";
import { createNewStudent, removeElement, renderListOfStudents } from "./render.js";

const lastName = document.querySelector('input[name="last-name"]');
const firstName = document.querySelector('input[name="first-name"]');
const patronymic = document.querySelector('input[name="patronymic"]');
const birthDate = document.querySelector('input[name="birth-date"]');
const group = document.querySelector('input[name="group"]');
const form = document.querySelector('.listing__input');

(async () => {
  try {
    const response = await getListOfAllStudents();
    renderListOfStudents(response)
  } catch (e) {
    alert(e);
    return;
  }
})();

form.addEventListener('submit', async (e) => {
  e.preventDefault();
  if (!lastName.value || !firstName.value || !patronymic.value || !birthDate.value || !group.value) {
    alert('Заполните все поля!');
    return;
  }
  try {
    const response = await saveNewStudent({
      lastName: lastName.value,
      firstName: firstName.value,
      patronymic: patronymic.value,
      birthDate: birthDate.value,
      group: group.value
    });
    createNewStudent(response);

    lastName.value = '';
    firstName.value = '';
    patronymic.value = '';
    birthDate.value = '';
    group.value = '';
  } catch (e) {
    alert(e);
    return;
  }
});
