CREATE DATABASE IF NOT EXISTS project_db;

USE project_db;



CREATE TABLE IF NOT EXISTS user_points (

    user VARCHAR(50) PRIMARY KEY,

    points INT DEFAULT 0,

    datetime_stamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP

);



-- Seed initial data for both server types to handle

INSERT INTO user_points (user, points) VALUES ('c_server_user1', 0) ON DUPLICATE KEY UPDATE user=user;
INSERT INTO user_points (user, points) VALUES ('python_server_user1', 0) ON DUPLICATE KEY UPDATE user=user;


CREATE USER IF NOT EXISTS 'root'@'%' IDENTIFIED BY '2GL@345';

GRANT ALL PRIVILEGES ON *.* TO 'root'@'%' WITH GRANT OPTION;

FLUSH PRIVILEGES;