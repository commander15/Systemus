CREATE TABLE Users (
	id       INTEGER PRIMARY KEY AUTOINCREMENT,
	name     VARCHAR(30) UNIQUE NOT NULL,
	password CHAR(60)    NOT NULL,
	active   BOOLEAN     DEFAULT FALSE,
	role_id  INTEGER     NOT NULL,
	CONSTRAINT FK_Users_00
	FOREIGN KEY (role_id) REFERENCES UserRoles(id)
);

CREATE TABLE UserProfiles (
	id         INTEGER PRIMARY KEY AUTOINCREMENT,
	name       VARCHAR(30) UNIQUE NOT NULL,
	first_name VARCHAR(15),
	sex        CHAR(1)     CHECK(sex = 'M' OR sex = 'F'),
	user_id    INTEGER NOT NULL,
	CONSTRAINT FK_UserProfiles_00
	FOREIGN KEY (user_id) REFERENCES Users(id)
);

CREATE TABLE UserRoles (
	id          INTEGER PRIMARY KEY AUTOINCREMENT,
	name        VARCHAR(30) UNIQUE NOT NULL,
	description VARCHAR(255)
);