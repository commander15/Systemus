CREATE TABLE SSystems (
    id      INTEGER     PRIMARY KEY DEFAULT 1 CHECK(id = 1),
    name    VARCHAR(32) UNIQUE NOT NULL,
    version VARCHAR(16) NOT NULL,
    icon    MEDIUMBLOB
);

CREATE TABLE SSystemSettings (
    id    INTEGER      PRIMARY KEY AUTO_INCREMENT,
    name  VARCHAR(32)  NOT NULL,
    value VARCHAR(128),
    type  INTEGER      NOT NULL
);

CREATE TABLE SSystemNotifications (
    id    INTEGER       PRIMARY KEY AUTO_INCREMENT,
    title VARCHAR(32)   NOT NULL,
    text  VARCHAR(255),
    data  VARCHAR(1024),
    type  VARCHAR(16)   DEFAULT 'Broadcast',
    date  DATE          NOT NULL,
    time  TIME          NOT NULL
);

CREATE TABLE SSystemInstallations (
    id      INTEGER     PRIMARY KEY AUTO_INCREMENT,
    version VARCHAR(10) NOT NULL,
    code    INTEGER     NOT NULL CHECK(code >= 0),
    date    DATE        NOT NULL,
    time    TIME        NOT NULL
);

CREATE TABLE SUsers (
    id            INTEGER      PRIMARY KEY AUTO_INCREMENT,
    login         VARCHAR(30)  UNIQUE NOT NULL,
    password      CHAR(60)     NOT NULL,
    description   VARCHAR(255),
    status        INTEGER      DEFAULT 0,
    logged_date   DATE,
    logged_time   TIME,
    creation_date DATE         NOT NULL,
    creation_time TIME         NOT NULL,
    role_id       INTEGER      NOT NULL
);

CREATE TABLE SProfiles (
    id         INTEGER     PRIMARY KEY AUTO_INCREMENT,
    name       VARCHAR(30) NOT NULL,
    first_name VARCHAR(15),
    sex        CHAR(1)     CHECK(sex = 'M' OR sex = 'F'),
    birthdate  DATE,
    phone      VARCHAR(12),
    email      VARCHAR(20),
    user_id    INTEGER     NOT NULL
);

CREATE TABLE SRoles (
    id            INTEGER      PRIMARY KEY AUTO_INCREMENT,
    name          VARCHAR(30)  UNIQUE NOT NULL,
    description   VARCHAR(255),
    creation_date DATE         NOT NULL,
    creation_time TIME         NOT NULL
);

CREATE TABLE SGroups (
    id            INTEGER      PRIMARY KEY AUTO_INCREMENT,
    name          VARCHAR(30)  UNIQUE NOT NULL,
    description   VARCHAR(255),
    creation_date DATE         NOT NULL,
    creation_time TIME         NOT NULL
);

CREATE TABLE SPrivileges (
    id            INTEGER      PRIMARY KEY AUTO_INCREMENT,
    name          VARCHAR(30)  UNIQUE NOT NULL,
    description   VARCHAR(255),
    creation_date DATE         NOT NULL,
    creation_time TIME         NOT NULL
);

CREATE TABLE SPermissions (
    id            INTEGER      PRIMARY KEY AUTO_INCREMENT,
    name          VARCHAR(30)  UNIQUE NOT NULL,
    description   VARCHAR(255),
    creation_date DATE         NOT NULL,
    creation_time TIME         NOT NULL
);

-- Relationships on SUsers

ALTER TABLE SUsers
ADD CONSTRAINT FK_SUsers_00
FOREIGN KEY(role_id) REFERENCES SRoles(id) ON UPDATE CASCADE ON DELETE CASCADE;

CREATE TABLE SUserGroups (
    id       INTEGER PRIMARY KEY AUTO_INCREMENT,
    add_date DATE    NOT NULL,
    add_time TIME    NOT NULL,
    user_id  INTEGER NOT NULL,
    group_id INTEGER NOT NULL,
    CONSTRAINT FK_SUserGroups_00
    FOREIGN KEY(user_id)  REFERENCES SUsers(id)  ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_SUserGroups_01
    FOREIGN KEY(group_id) REFERENCES SGroups(id) ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE SUserPrivileges (
    id           INTEGER PRIMARY KEY AUTO_INCREMENT,
    issue_date   DATE    NOT NULL,
    issue_time   TIME    NOT NULL,
    privilege_id INTEGER NOT NULL,
    user_id      INTEGER NOT NULL,
    CONSTRAINT FK_SUserPrivileges_00
    FOREIGN KEY(privilege_id) REFERENCES SPrivileges(id) ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_SUserPrivileges_01
    FOREIGN KEY(user_id)      REFERENCES SUsers(id)      ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE SUserPermissions (
    id            INTEGER PRIMARY KEY AUTO_INCREMENT,
    issue_date    DATE    NOT NULL,
    issue_time    TIME    NOT NULL,
    permission_id INTEGER NOT NULL,
    user_id       INTEGER NOT NULL,
    CONSTRAINT FK_SUserPermissions_00
    FOREIGN KEY(permission_id) REFERENCES SPermissions(id) ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_SUserPermissions_01
    FOREIGN KEY(user_id)       REFERENCES SUsers(id)       ON UPDATE CASCADE ON DELETE CASCADE
);

-- Relationships on SProfiles

ALTER TABLE SProfiles
ADD CONSTRAINT FK_SProfiles_00
FOREIGN KEY(user_id) REFERENCES SUsers(id) ON UPDATE CASCADE ON DELETE CASCADE;

-- Relationships on SRoles

CREATE TABLE SRolePrivileges (
    id           INTEGER PRIMARY KEY AUTO_INCREMENT,
    issue_date   DATE    NOT NULL,
    issue_time   TIME    NOT NULL,
    privilege_id INTEGER NOT NULL,
    role_id      INTEGER NOT NULL,
    CONSTRAINT FK_SRolePrivileges_00
    FOREIGN KEY(privilege_id) REFERENCES SPrivileges(id) ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_SRolePrivileges_01
    FOREIGN KEY(role_id)      REFERENCES SRoles(id)      ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE SRolePermissions (
    id            INTEGER PRIMARY KEY AUTO_INCREMENT,
    issue_date    DATE    NOT NULL,
    issue_time    TIME    NOT NULL,
    permission_id INTEGER NOT NULL,
    role_id       INTEGER NOT NULL,
    CONSTRAINT FK_SRolePermissions_00
    FOREIGN KEY(permission_id) REFERENCES SPermissions(id) ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_SRolePermissions_01
    FOREIGN KEY(role_id)       REFERENCES SRoles(id)       ON UPDATE CASCADE ON DELETE CASCADE
);

-- Relationships on SGroups

CREATE TABLE SGroupPrivileges (
    id           INTEGER PRIMARY KEY AUTO_INCREMENT,
    issue_date   DATE    NOT NULL,
    issue_time   TIME    NOT NULL,
    privilege_id INTEGER NOT NULL,
    group_id     INTEGER NOT NULL,
    CONSTRAINT FK_SGroupPrivileges_00
    FOREIGN KEY(privilege_id) REFERENCES  SPrivileges(id) ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_SGroupPrivileges_01
    FOREIGN KEY(group_id)     REFERENCES  SGroups(id)     ON UPDATE CASCADE ON DELETE CASCADE
);

CREATE TABLE SGroupPermissions (
    id            INTEGER PRIMARY KEY AUTO_INCREMENT,
    issue_date    DATE    NOT NULL,
    issue_time    TIME    NOT NULL,
    permission_id INTEGER NOT NULL,
    group_id      INTEGER NOT NULL,
    CONSTRAINT FK_SGroupPermissions_00
    FOREIGN KEY(permission_id) REFERENCES  SPermissions(id) ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_SGroupPermissions_01
    FOREIGN KEY(group_id)      REFERENCES  SGroups(id)      ON UPDATE CASCADE ON DELETE CASCADE
);

-- Relationships on SPrivileges

CREATE TABLE SPrivilegePermissions (
    id            INTEGER PRIMARY KEY AUTO_INCREMENT,
    issue_date    DATE    NOT NULL,
    issue_time    TIME    NOT NULL,
    permission_id INTEGER NOT NULL,
    privilege_id  INTEGER NOT NULL,
    CONSTRAINT FK_SPrivilegePermissions_00
    FOREIGN KEY(permission_id) REFERENCES SPermissions(id) ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT FK_SPrivilegePermissions_01
    FOREIGN KEY(privilege_id)  REFERENCES SPrivileges(id)  ON UPDATE CASCADE ON DELETE CASCADE
);

