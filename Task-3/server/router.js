import { Router } from 'express';
import controller from './controllers.js';

export const router = new Router();

router.get('/', (req, res) => controller.getAllTasks(req, res));
router.get('/find/:searchString', (req, res) => controller.getTasksBySearchString(req, res));
router.get('/date', (req, res) => controller.getTasksByDateAndStatus(req, res));
