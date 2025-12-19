#include "Note.hpp"

#include <ranges>

Note::Note(Id id, const std::string& title) : m_note_data({id, 0, title, {}, 0, {},}) {
}

Note::Note(Id id, const std::string& title, const std::string& text) : m_note_data({id, 0, title, text, 0, {}}) {

}

void Note::addUser(const User& user) {
//    m_current_users.emplace_back(std::make_shared<User>(user));
}

void Note::removeUser(Id user_id)
{
//    std::erase_if(m_current_users, [user_id](const auto& user){
//        return user->getId() == user_id;
//    });
}

void Note::incrementVersion() {
    m_note_data.version++;
}

// Запросы клиента

// AUTH_REQUEST
// 1) Регистрация/авторизация пользователя
// длина логина
// длина пароля
// логин
// пароль

// NOTES_LIST
// 2) Получение списка заметок
// айдишник пользователя

// NOTE_CREATE
// 3) Добавление новой заметки
// айдишник пользователя
// текст, на случай ошибки создания заметки

// 4) Отправка данных о добавлении текста в конец
// айдшник пользователя
// айлишник документа
// добавленный текст

// 5) Отправка данных о удалении текста
// позиция начала удаления
// позиция конца удаления
// айдишник документа
// айдишниу пользователя

// 6) вставка текста
// начальная позиция вставки
// величина текста для вставки
// айдишник пользователя
// айдишник документа

// Ответы сервера
// 1) флаг того, что пользователь найден или нет

// 2) тут вопрос в каком виде передавать информацию о заметках

// 3)


