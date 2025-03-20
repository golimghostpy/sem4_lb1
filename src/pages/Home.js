import React, { useEffect, useState } from 'react';
import { Link } from 'react-router-dom';
import axios from 'axios';
import './styles.css'; // Import the common CSS file

const Home = () => {
  const [data, setData] = useState([]);

  const loadData = async () => {
    try {
      const response = await axios.get("http://localhost:5000/items");
      setData(response.data);
      console.log("Данные загружены:", response.data);
    } catch (error) {
      console.error("Ошибка запроса:", error);
    }
  };

  useEffect(() => {
    loadData();
  }, []);

  const deleteItem = async (id) => {
    try {
      await axios.delete(`http://localhost:5000/items/${id}`);
      console.log(`Товар с id ${id} успешно удален`);
      setData(data.filter(item => item.id !== id));
    } catch (error) {
      console.error(`Ошибка удаления товара с id ${id}:`, error);
    }
  };

  return (
    <div className="container home-container">
      <h1>Каталог отелей</h1>
      <ul>
        {data.map(item => (
          <li key={item.id}>
            <Link to={`/detail/${item.id}`} style={{ display: 'flex', alignItems: 'center' }}>
              {item.name}
            </Link>
            <button onClick={() => deleteItem(item.id)}>
              Удалить
            </button>
          </li>
        ))}
      </ul>
      <Link to="/add" className="add-link">Добавить отель</Link>
    </div>
  );
};

export default Home;
