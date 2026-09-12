create table employees (
    employee_id string,
    full_name string,
    gender string,
    birth_date string,
    phone_number string,
    email_address string,
    department string,
    job_title string,
    hire_date string,
    salary string,
    address string,
    city string,
    province string,
    postal_code string,
    emergency_contact string,
    emergency_phone string,
    bank_account string,
    id_card_number string
);

insert employees(employee_id,full_name,gender,birth_date,phone_number,email_address,department,job_title,hire_date,salary,address,city,province,postal_code,emergency_contact,emergency_phone,bank_account,id_card_number):('e001','zhangwei','m','19900512','13800000001','zhangwei@company.com','rd','seniorengineer','20200301','25000','changshayuelu100','changsha','hunan','410000','lijing','13900000001','6222020200000001','430104199005120001');
insert employees(employee_id,full_name,gender,birth_date,phone_number,email_address,department,job_title,hire_date,salary,address,city,province,postal_code,emergency_contact,emergency_phone,bank_account,id_card_number):('e002','laoda','m','19900512','13800000001','zhangwei@company.com','rd','seniorengineer','20200301','25000','changshayuelu100','changsha','hunan','410000','lijing','13900000001','6222020200000001','430104199005120001');
insert employees(full_name , address):('wangzi' , 'tablein');
create table user(id string , name string , age string);

insert user(id):('1');
