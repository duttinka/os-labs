#pragma once

// Максимальная длина сообщения 20 символов
const int MESSAGE_MAX_LEN = 20;

struct Message {
  char text[MESSAGE_MAX_LEN];
};