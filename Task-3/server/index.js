import express from 'express';
import cors from 'cors';
import { router } from './router.js';

const app = express();
const PORT = 3333;

app.use(cors());
app.use(express.json());
app.use('/api', router);

app.listen(PORT, () => {
  console.log(`Server has been started on ${PORT} port...`)
});
