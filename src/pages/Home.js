import React, { useEffect, useState } from 'react';
import { Link } from 'react-router-dom';
import axios from 'axios';

const Home = () => {
  const [data, setData] = useState([]);

  // Функция для загрузки данных
  const loadData = async () => {
    try {
      const response = await axios.get("http://localhost:5000/items");
      setData(response.data);
      console.log("Данные загружены:", response.data);
    } catch (error) {
      console.error("Ошибка запроса:", error);
    }
  };

  // Загружаем данные при монтировании компонента
  useEffect(() => {
    loadData();
  }, []);

  // Функция для удаления товара
  const deleteItem = async (id) => {
    try {
      await axios.delete(`http://localhost:5000/items/${id}`);
      console.log(`Товар с id ${id} успешно удален`);
      // Обновляем состояние данных
      setData(data.filter(item => item.id !== id));
    } catch (error) {
      console.error(`Ошибка удаления товара с id ${id}:`, error);
    }
  };

  return (
    <div>
      <h1>Список товаров</h1>
      <ul>
        {data.map(item => (
          <li key={item.id}>
            <Link to={`/detail/${item.id}`}>{item.name}</Link>
            <button onClick={() => deleteItem(item.id)} style={{ marginLeft: "10px" }}>
              Удалить
            </button>
          </li>
        ))}
      </ul>
      <Link to="/add">Добавить товар</Link>
    </div>
  );
};

export default Home;
