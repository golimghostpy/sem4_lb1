import React, { useEffect, useRef, useState } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import axios from 'axios';
import './styles.css';

const Detail = () => {
    const { id } = useParams();
    const navigate = useNavigate();
    const [itemData, setItemData] = useState({});
    const nameRef = useRef(null);
    const cityRef = useRef(null);
    const ratingRef = useRef(null);
    const descriptionRef = useRef(null);

    useEffect(() => {
        const loadItem = async () => {
            try {
                const response = await axios.get(`http://localhost:5000/items/${id}`);
                setItemData(response.data);
                console.log("Загруженный товар:", response.data);
            } catch (error) {
                console.error("Ошибка загрузки:", error);
            }
        };

        loadItem();
    }, [id]);

    useEffect(() => {
        if (nameRef.current && itemData) {
            nameRef.current.value = itemData.name || '';
        }
        if (cityRef.current && itemData) {
            cityRef.current.value = itemData.city || '';
        }
        if (ratingRef.current && itemData) {
            ratingRef.current.value = itemData.rating || '';
        }
        if (descriptionRef.current && itemData) {
            descriptionRef.current.value = itemData.description || '';
        }
    }, [itemData]);

    const handleSubmit = (e) => {
        e.preventDefault();
        const updatedItem = {
            name: nameRef.current.value,
            city: cityRef.current.value,
            rating: ratingRef.current.value || undefined,
            description: descriptionRef.current.value || undefined,
        };

        axios.put(`http://localhost:5000/items/${id}`, updatedItem, {
            headers: { "Content-Type": "application/json" }
        })
            .then(response => {
                setItemData(response.data);
                console.log("Обновленный товар:", response.data);
                navigate('/');
            })
            .catch(error => console.error("Ошибка обновления:", error));
    };

    return (
        <div className="container detail-container">
            <h1>{'Информация об отеле ' + itemData.name}</h1>
            <form onSubmit={handleSubmit}>
                <label>
                    Название:
                    <input type="text" ref={nameRef} required />
                </label>
                <label>
                    Город:
                    <input type="text" ref={cityRef} />
                </label>
                <label>
                    Описание:
                    <textarea
                        ref={descriptionRef}
                    ></textarea>
                </label>
                <label>
                    Оценка:
                    <input type="number" ref={ratingRef} min="1" max="5" />
                </label>
                <button type="submit">Сохранить</button>
            </form>
        </div>
    );
};

export default Detail;
