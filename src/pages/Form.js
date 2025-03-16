import React from 'react';
import axios from 'axios';
import { useNavigate } from 'react-router-dom';

const Form = () => {
    let name = ''; // Используем обычную переменную
    const navigate = useNavigate();

    const handleChange = (e) => {
        name = e.target.value; 
        console.log("Текущее значение name:", name); 
    };

    const handleSubmit = (e) => {
        e.preventDefault();
        axios.post('http://localhost:5000/items', { name })
            .then(() => navigate('/'))
            .catch(error => console.error(error));
    };

    return (
        <form onSubmit={handleSubmit}>
            <input type="text" onChange={handleChange} required />
            <button type="submit">Сохранить</button>
        </form>
    );
};

export default Form;
