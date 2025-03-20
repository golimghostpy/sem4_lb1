import React from 'react';
import { BrowserRouter as Router, Route, Routes } from 'react-router-dom';
import Home from './pages/Home';
import Detail from './pages/Detail';
import Form from './pages/Form';
import Authentication from './pages/Authentication';
import Registration from './pages/Registration';
import { AuthProvider } from './pages/AuthContext';
import ProtectedRoute from './ProtectedRoute';

const App = () => {
    return (
        <AuthProvider>
            <Router>
                <Routes>
                    <Route path="/login" element={<Authentication />} />
                    <Route path="/register" element={<Registration />} />
                    <Route
                        path="/"
                        element={<ProtectedRoute element={<Home />} />}
                    />
                    <Route
                        path="/detail/:id"
                        element={<ProtectedRoute element={<Detail />} />}
                    />
                    <Route
                        path="/add"
                        element={<ProtectedRoute element={<Form />} />}
                    />
                </Routes>
            </Router>
        </AuthProvider>
    );
};

export default App;
