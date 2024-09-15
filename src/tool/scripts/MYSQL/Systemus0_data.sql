/* System data */

START TRANSACTION;

INSERT INTO SSystems (name, version)
VALUES ('Systemus', '1.0.0');

INSERT INTO SSystemInstallations (version, code, date, time)
VALUES ('1.0.0', 0, CURRENT_DATE, CURRENT_TIME);

/* Admin privileges, permissions, role and groups */

-- User management

INSERT INTO SPrivileges (name, description, creation_date, creation_time)
VALUES ('user.management', 'Users management privilege.', CURRENT_DATE, CURRENT_TIME);

SET @user_privilege_id := LAST_INSERT_ID();

INSERT INTO SPermissions (name, description, creation_date, creation_time)
VALUES
    ('user.list', 'List users.', CURRENT_DATE, CURRENT_TIME),
    ('user.create', 'Create a user.', CURRENT_DATE, CURRENT_TIME),
    ('user.read', 'Show user data.', CURRENT_DATE, CURRENT_TIME),
    ('user.status.change', 'Change user status.', CURRENT_DATE, CURRENT_TIME),
    ('user.password.reset', 'Reset user password.', CURRENT_DATE, CURRENT_TIME),
    ('user.delete', 'Delete user data.', CURRENT_DATE, CURRENT_TIME);

SET @user_permission_id := LAST_INSERT_ID();

-- Admin management

INSERT INTO SPrivileges (name, description, creation_date, creation_time)
VALUES ('admin.management', 'Administrators management privilege.', CURRENT_DATE, CURRENT_TIME);

SET @admin_privilege_id := LAST_INSERT_ID();

INSERT INTO SPermissions (name, description, creation_date, creation_time)
VALUES
    ('admin.list', 'List administrators.', CURRENT_DATE, CURRENT_TIME),
    ('admin.create', 'Create an administrator.', CURRENT_DATE, CURRENT_TIME),
    ('admin.read', 'Show administrator data.', CURRENT_DATE, CURRENT_TIME),
    ('admin.status.change', 'Change administrator status.', CURRENT_DATE, CURRENT_TIME),
    ('admin.password.reset', 'Reset administrator password.', CURRENT_DATE, CURRENT_TIME),
    ('admin.delete', 'Delete administrator data.', CURRENT_DATE, CURRENT_TIME);

SET @admin_permission_id := LAST_INSERT_ID();

-- Admin and user management impl

INSERT INTO SPrivilegePermissions (issue_date, issue_time, permission_id, privilege_id)
VALUES
    (CURRENT_DATE, CURRENT_TIME, @user_permission_id + 0,  @user_privilege_id),
    (CURRENT_DATE, CURRENT_TIME, @user_permission_id + 1,  @user_privilege_id),
    (CURRENT_DATE, CURRENT_TIME, @user_permission_id + 2,  @user_privilege_id),
    (CURRENT_DATE, CURRENT_TIME, @user_permission_id + 3,  @user_privilege_id),
    (CURRENT_DATE, CURRENT_TIME, @user_permission_id + 4,  @user_privilege_id),
    (CURRENT_DATE, CURRENT_TIME, @user_permission_id + 5,  @user_privilege_id),
    (CURRENT_DATE, CURRENT_TIME, @admin_permission_id + 0, @admin_privilege_id),
    (CURRENT_DATE, CURRENT_TIME, @admin_permission_id + 1, @admin_privilege_id),
    (CURRENT_DATE, CURRENT_TIME, @admin_permission_id + 2, @admin_privilege_id),
    (CURRENT_DATE, CURRENT_TIME, @admin_permission_id + 3, @admin_privilege_id),
    (CURRENT_DATE, CURRENT_TIME, @admin_permission_id + 4, @admin_privilege_id),
    (CURRENT_DATE, CURRENT_TIME, @admin_permission_id + 5, @admin_privilege_id);

-- Admin role

INSERT INTO SRoles (name, description, creation_date, creation_time)
VALUES ('Administrator', 'System super administrator.', CURRENT_DATE, CURRENT_TIME);

SET @admin_role_id := LAST_INSERT_ID();

INSERT INTO SRolePrivileges (issue_date, issue_time, privilege_id, role_id)
VALUES (CURRENT_DATE, CURRENT_TIME, @admin_privilege_id, @admin_role_id);

-- Admin group

INSERT INTO SGroups (name, description, creation_date, creation_time)
VALUES ('Administrators', 'System administrators.', CURRENT_DATE, CURRENT_TIME);

SET @admin_group_id := LAST_INSERT_ID();

INSERT INTO SGroupPrivileges (issue_date, issue_time, privilege_id, group_id)
VALUES (CURRENT_DATE, CURRENT_TIME, @user_privilege_id, @admin_group_id);

/* Admin user */

INSERT INTO SUsers (login, password, description, creation_date, creation_time, role_id)
VALUES ('admin', '$2a$12$qepK3WfCCtrH8pyr5R.tGedFDNX.kCh7o4.bqPfqmBjPsyyVzs0Hi', 'System main administrator', CURRENT_DATE, CURRENT_TIME, @admin_role_id);

INSERT INTO SProfiles (name, user_id)
VALUES ('Administrator', LAST_INSERT_ID());

COMMIT;
