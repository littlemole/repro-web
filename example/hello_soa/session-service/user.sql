CREATE TABLE users (
    id INT AUTO_INCREMENT NOT_NULL UNIQUE PRIMARY KEY,
    username VARCHAR(256) NOT NULL,
    login VARCHAR(64) NOT NULL UNIQUE,
    pwd VARCHAR(270) NOT NULL,
    avatar_url VARCHAR(2048)
);
