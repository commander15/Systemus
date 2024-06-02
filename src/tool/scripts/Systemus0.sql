CREATE TABLE IF NOT EXISTS Systems (
    id      INTEGER     PRIMARY KEY AUTO_INCREMENT,
    name    VARCHAR(20) NOT NULL,
    version VARCHAR(12) NOT NULL
);

CREATE TABLE IF NOT EXISTS SystemInstallations (
    id        INTEGER     PRIMARY KEY AUTO_INCREMENT,
    version   VARCHAR(12) NOT NULL,
    date      DATE        NOT NULL,
    time      TIME        NOT NULL,
    system_id INTEGER     NOT NULL
);

CREATE TABLE IF NOT EXISTS Users (
    id         INTEGER     PRIMARY KEY AUTO_INCREMENT,
    login      VARCHAR(30) UNIQUE NOT NULL,
    password   VARCHAR(60) NOT NULL,
    active     BOOLEAN     DEFAULT TRUE,
    profile_id INTEGER     NOT NULL,
    role_id    INTEGER     NOT NULL
);

CREATE TABLE IF NOT EXISTS Profiles (
    id         INTEGER     PRIMARY KEY AUTO_INCREMENT,
    name       VARCHAR(30) NOT NULL,
    first_name VARCHAR(15),
    sex        CHAR(1)     CHECK(sex = 'M' OR sex = 'F'),
    birthdate  DATE,
    phone      VARCHAR(12),
    email      VARCHAR(20)
);

CREATE TABLE IF NOT EXISTS Roles (
    id          INTEGER      PRIMARY KEY AUTO_INCREMENT,
    name        VARCHAR(30)  UNIQUE NOT NULL,
    description VARCHAR(255)
);

CREATE TABLE IF NOT EXISTS Groups (
    id          INTEGER      PRIMARY KEY AUTO_INCREMENT,
    name        VARCHAR(30)  UNIQUE NOT NULL,
    description VARCHAR(255)
);

CREATE TABLE IF NOT EXISTS Privileges (
    id          INTEGER      PRIMARY KEY AUTO_INCREMENT,
    name        VARCHAR(30)  UNIQUE NOT NULL,
    description VARCHAR(255)
);

CREATE TABLE IF NOT EXISTS Permissions (
    id          INTEGER      PRIMARY KEY AUTO_INCREMENT,
    name        VARCHAR(30)  UNIQUE NOT NULL,
    description VARCHAR(255)
);

/* Relationships on Systems */

ALTER TABLE SystemInstallations
ADD CONSTRAINT FK_SystemInstallations_00
FOREIGN KEY(system_id) REFERENCES Systems(id) ON UPDATE CASCADE ON DELETE CASCADE;

/* Relationships on Users */

ALTER TABLE Users
ADD CONSTRAINT FK_Users_00
FOREIGN KEY(profile_id) REFERENCES Profiles(id) ON UPDATE CASCADE ON DELETE CASCADE;

ALTER TABLE Users
ADD CONSTRAINT FK_Users_01
FOREIGN KEY(role_id) REFERENCES Roles(id) ON UPDATE CASCADE ON DELETE CASCADE;

CREATE TABLE IF NOT EXISTS UserGroups (
    id       INTEGER PRIMARY KEY AUTO_INCREMENT,
    user_id  INTEGER NOT NULL,
    group_id INTEGER NOT NULL,
    CONSTRAINT FK_UserGroups_00
    FOREIGN KEY(user_id)  REFERENCES Users(id)  ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_UserGroups_01
    FOREIGN KEY(group_id) REFERENCES Groups(id) ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS UserPrivileges (
    id           INTEGER PRIMARY KEY AUTO_INCREMENT,
    active       BOOLEAN DEFAULT TRUE,
    privilege_id INTEGER NOT NULL,
    user_id      INTEGER NOT NULL,
    CONSTRAINT FK_UserPrivileges_00
    FOREIGN KEY(privilege_id) REFERENCES Privileges(id) ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_UserPrivileges_01
    FOREIGN KEY(user_id)      REFERENCES Users(id)      ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS UserPermissions (
    id            INTEGER PRIMARY KEY AUTO_INCREMENT,
    active        BOOLEAN DEFAULT TRUE,
    permission_id INTEGER NOT NULL,
    user_id       INTEGER NOT NULL,
    CONSTRAINT FK_UserPermissions_00
    FOREIGN KEY(permission_id) REFERENCES Permissions(id) ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_UserPermissions_01
    FOREIGN KEY(user_id)       REFERENCES Users(id)       ON UPDATE CASCADE ON DELETE CASCADE
);

/* Relationships on Roles */

CREATE TABLE IF NOT EXISTS RolePrivileges (
    id           INTEGER PRIMARY KEY AUTO_INCREMENT,
    active       BOOLEAN DEFAULT TRUE,
    privilege_id INTEGER NOT NULL,
    role_id      INTEGER NOT NULL,
    CONSTRAINT FK_RolePrivileges_00
    FOREIGN KEY(privilege_id) REFERENCES Privileges(id) ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_RolePrivileges_01
    FOREIGN KEY(role_id)      REFERENCES Roles(id)      ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS RolePermissions (
    id            INTEGER PRIMARY KEY AUTO_INCREMENT,
    active        BOOLEAN DEFAULT TRUE,
    permission_id INTEGER NOT NULL,
    role_id       INTEGER NOT NULL,
    CONSTRAINT FK_RolePermissions_00
    FOREIGN KEY(permission_id) REFERENCES Permissions(id) ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_RolePermissions_01
    FOREIGN KEY(role_id)       REFERENCES Roles(id)       ON UPDATE CASCADE ON DELETE CASCADE
);

/* Relationships on Groups */

CREATE TABLE IF NOT EXISTS GroupPrivileges (
    id           INTEGER PRIMARY KEY AUTO_INCREMENT,
    active       BOOLEAN DEFAULT TRUE,
    privilege_id INTEGER NOT NULL,
    group_id     INTEGER NOT NULL,
    CONSTRAINT FK_GroupPrivileges_00
    FOREIGN KEY(privilege_id) REFERENCES Privileges(id) ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_GroupPrivileges_01
    FOREIGN KEY(group_id)      REFERENCES Groups(id)    ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE IF NOT EXISTS GroupPermissions (
    id            INTEGER PRIMARY KEY AUTO_INCREMENT,
    active        BOOLEAN DEFAULT TRUE,
    permission_id INTEGER NOT NULL,
    group_id      INTEGER NOT NULL,
    CONSTRAINT FK_GroupPermissions_00
    FOREIGN KEY(permission_id) REFERENCES Permissions(id) ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_GroupPermissions_01
    FOREIGN KEY(group_id)       REFERENCES Groups(id)     ON UPDATE CASCADE ON DELETE CASCADE
);

/* Relationships on Privileges */

CREATE TABLE IF NOT EXISTS PrivilegePermissions (
    id            INTEGER PRIMARY KEY AUTO_INCREMENT,
    active        BOOLEAN DEFAULT TRUE,
    permission_id INTEGER NOT NULL,
    privilege_id  INTEGER NOT NULL,
    CONSTRAINT FK_PrivilegePermissions_00
    FOREIGN KEY(permission_id) REFERENCES Permissions(id) ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_PrivilegePermissions_01
    FOREIGN KEY(privilege_id)  REFERENCES Privileges(id)  ON UPDATE CASCADE ON DELETE CASCADE
);

