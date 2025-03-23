import React, { useState } from 'react';
import axios from 'axios';
import { useNavigate } from 'react-router-dom';
import './styles.css';

const Registration = () => {
    const [login, setLogin] = useState('');
    const [password, setPassword] = useState('');
    const [error, setError] = useState('');
    const navigate = useNavigate();

    const handleLoginChange = (e) => {
        setLogin(e.target.value);
    };

    const handlePasswordChange = (e) => {
        setPassword(e.target.value);
    };

    const handleSubmit = async (e) => {
        e.preventDefault();
        try {
            const response = await axios.get('http://localhost:5000/users');
            const userExists = response.data.some(user => user.login === login);
            if (userExists) {
                setError('Пользователь с таким именем уже существует.');
            } else {
                await axios.post('http://localhost:5000/users', { login, password });
                console.log('Registration successful');
                navigate('/login');
            }
        } catch (error) {
            console.error('Registration failed:', error);
            setError('Ошибка регистрации. Пожалуйста, попробуйте снова.');
        }
    };

    return (
        <div className="container auth-container">
            <h2>Регистрация</h2>
            {error && <p style={{ color: 'red' }}>{error}</p>}
            <form onSubmit={handleSubmit}>
                <div className="form-group">
                    <label>Логин:</label>
                    <input
                        type="text"
                        value={login}
                        onChange={handleLoginChange}
                        required
                    />
                </div>
                <div className="form-group">
                    <label>Пароль:</label>
                    <input
                        type="password"
                        value={password}
                        onChange={handlePasswordChange}
                        required
                    />
                </div>
                <button type="submit">Зарегистрироваться</button>
            </form>
        </div>
    );
};

export default Registration;
