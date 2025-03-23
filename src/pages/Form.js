import React, { useState } from 'react';
import axios from 'axios';
import { useNavigate } from 'react-router-dom';
import './styles.css';

const Form = () => {
    const [name, setName] = useState('');
    const [description, setDescription] = useState('');
    const [city, setCity] = useState('');
    const navigate = useNavigate();

    const handleChangeName = (e) => {
        setName(e.target.value);
    };

    const handleChangeDescription = (e) => {
        setDescription(e.target.value);
    };

    const handleChangeCity = (e) => {
        setCity(e.target.value);
    };

    const handleSubmit = (e) => {
        e.preventDefault();
        axios.post('http://localhost:5000/items', { name, description, city })
            .then(() => navigate('/'))
            .catch(error => console.error(error));
        console.log("Отель", name, "был добавлен");
    };

    return (
        <div className="container form-container">
            <h1>Добавление отеля</h1>
            <form onSubmit={handleSubmit}>
                <div className="form-group">
                    <label>
                        Название:
                        <input type="text" value={name} onChange={handleChangeName} required />
                    </label>
                </div>
                <div className="form-group">
                    <label>
                        Город:
                        <input type="text" value={city} onChange={handleChangeCity} required />
                    </label>
                </div>
                <div className="form-group">
                    <label>
                        Описание:
                        <textarea value={description} onChange={handleChangeDescription} />
                    </label>
                </div>
                <button type="submit">Сохранить</button>
            </form>
        </div>
    );
};

export default Form;
