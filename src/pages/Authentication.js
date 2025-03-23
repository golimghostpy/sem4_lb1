import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import axios from 'axios';
import { useAuth } from './AuthContext';
import './styles.css';

const Authentication = () => {
    const [login, setLogin] = useState('');
    const [password, setPassword] = useState('');
    const [error, setError] = useState('');
    const navigate = useNavigate();
    const { login: authLogin } = useAuth();

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
            console.log(response.data);
            const user = response.data.find(user => user.login === login && user.password === password);
            if (user) {
                authLogin();
                navigate('/');
            } else {
                setError('Неверный логин или пароль.');
            }
        } catch (error) {
            console.error('Ошибка входа:', error);
            setError('Ошибка входа. Пожалуйста, попробуйте снова.');
        }
    };

    const handleRegisterClick = () => {
        navigate('/register');
    };

    return (
        <div className="container auth-container">
            <h2>Вход</h2>
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
                <button type="submit">Войти</button>
            </form>
            <button className="register-button" onClick={handleRegisterClick}>Регистрация</button>
        </div>
    );
};

export default Authentication;
