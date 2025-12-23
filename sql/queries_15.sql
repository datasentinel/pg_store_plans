SET client_min_messages=INFO;

\set ECHO all   
\unset QUIET
\pset tuples_only on
\pset recordsep '\n\n=======\n'
\echo  ###### format conversion tests

-- ===== Setup Test Environment =====
CREATE TABLE customers (
    customer_id SERIAL PRIMARY KEY,
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    address_id INT NOT NULL,
    email VARCHAR(150),
    phone_number VARCHAR(20),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

create index customers_idx on customers(last_name);


CREATE TABLE regions (
    region_id SERIAL PRIMARY KEY,
    region_name VARCHAR(100) NOT NULL
);

CREATE TABLE countries (
    country_id SERIAL PRIMARY KEY,
    country_name VARCHAR(100) NOT NULL,
    region_id INT NOT NULL,
    FOREIGN KEY (region_id) REFERENCES regions(region_id)
);

CREATE TABLE products (
    product_id SERIAL PRIMARY KEY,
    product_name VARCHAR(200) NOT NULL,
    product_description TEXT,
    price DECIMAL(10, 2) NOT NULL,
    stock_quantity INT NOT NULL
);

CREATE TABLE branches (
    branch_id SERIAL PRIMARY KEY,
    branch_name VARCHAR(150) NOT NULL,
    branch_location VARCHAR(200)
);

CREATE TABLE employees (
    employee_id SERIAL PRIMARY KEY,
    employee_name VARCHAR(150) NOT NULL,
    branch_id INT NOT NULL,
    position VARCHAR(100),
    hire_date DATE NOT NULL,
    FOREIGN KEY (branch_id) REFERENCES branches(branch_id)
);


CREATE TABLE orders (
    order_id SERIAL,
    customer_id INT NOT NULL,
    employee_id INT NOT NULL,
    order_date DATE NOT NULL,
    total_amount DECIMAL(10, 2),
    FOREIGN KEY (customer_id) REFERENCES customers(customer_id),
    FOREIGN KEY (employee_id) REFERENCES employees(employee_id),
    PRIMARY KEY (order_date,order_id)
) PARTITION BY RANGE (order_date);

create index order_id_idx on orders(order_id);

DO $$
DECLARE
    i INT;
BEGIN
    FOR i IN 0..19 LOOP
        EXECUTE format('CREATE TABLE orders_p%s PARTITION OF orders FOR VALUES FROM (%L) TO (%L)', 
            i, CURRENT_DATE - i, CURRENT_DATE - i + 1);
    END LOOP;
    CREATE TABLE orders_default PARTITION OF orders DEFAULT;
END $$;

CREATE TABLE order_details (
    order_detail_id SERIAL,
    order_id INT NOT NULL,
    product_id INT NOT NULL,
    quantity INT NOT NULL,
    price DECIMAL(10, 2) NOT NULL,
    FOREIGN KEY (product_id) REFERENCES products(product_id),
    PRIMARY KEY (order_detail_id, order_id)
) PARTITION BY HASH (order_id);

CREATE TABLE order_details_p0 PARTITION OF order_details FOR VALUES WITH (MODULUS 5, REMAINDER 0);
CREATE TABLE order_details_p1 PARTITION OF order_details FOR VALUES WITH (MODULUS 5, REMAINDER 1);
CREATE TABLE order_details_p2 PARTITION OF order_details FOR VALUES WITH (MODULUS 5, REMAINDER 2);
CREATE TABLE order_details_p3 PARTITION OF order_details FOR VALUES WITH (MODULUS 5, REMAINDER 3);
CREATE TABLE order_details_p4 PARTITION OF order_details FOR VALUES WITH (MODULUS 5, REMAINDER 4);


CREATE TABLE payments (
    payment_id SERIAL PRIMARY KEY,
    order_id INT NOT NULL,
    payment_date DATE NOT NULL,
    amount DECIMAL(10, 2) NOT NULL,
    payment_method VARCHAR(50)
);

CREATE TABLE addresses (
    address_id SERIAL PRIMARY KEY,
    street_address VARCHAR(200) NOT NULL,
    city VARCHAR(100) NOT NULL,
    state VARCHAR(100),
    country_id INT NOT NULL,
    postal_code VARCHAR(20),
    FOREIGN KEY (country_id) REFERENCES countries(country_id)
);

-- ===== Insert Sample Data =====
INSERT INTO regions (region_name) SELECT 'Region_' || i FROM GENERATE_SERIES(1, 10) AS i; -- 10 regions;
INSERT INTO countries (country_name, region_id)  SELECT 'Country_' || i, CEIL(RANDOM() * 10)
FROM GENERATE_SERIES(1, 100) AS i; -- 100 countries, random regions
INSERT INTO addresses (street_address, city, state, country_id, postal_code)
SELECT
    'Street_' || i,
    'City_' || CEIL(RANDOM() * 1000),
    'State_' || CEIL(RANDOM() * 50),
    CEIL(RANDOM() * 100),
    '10000' || i
FROM GENERATE_SERIES(1, 1000) AS i; -- 1000 addresses
INSERT INTO branches (branch_name, branch_location)
SELECT
    'Branch_' || i,
    'Location_' || CEIL(RANDOM() * 100)
FROM GENERATE_SERIES(1, 50) AS i; -- 50 branches
INSERT INTO employees (employee_name, branch_id, position, hire_date)
SELECT
    'Employee_' || i,
    CEIL(RANDOM() * 50),
    'Position_' || CEIL(RANDOM() * 10),
    CURRENT_DATE - INTERVAL '1 day' * FLOOR(RANDOM() * 365 * 5) -- Random hire date in the past 5 years
FROM GENERATE_SERIES(1, 500) AS i; -- 500 employees
INSERT INTO customers (first_name, last_name, address_id, email, phone_number, created_at)
SELECT
    'FirstName' || i,
    'LastName' || i,
    CEIL(RANDOM() * 1000), -- Random address_id
    'customer' || i || '@example.com',
    '123-456-7890',
    CURRENT_DATE - INTERVAL '1 day' * FLOOR(RANDOM() * 365 * 2) -- Random creation date in the past 2 years
FROM GENERATE_SERIES(1, 1000) AS i;
INSERT INTO orders (customer_id, employee_id, order_date, total_amount)
SELECT
    CEIL(RANDOM() * 1000), -- Random customer_id
    CEIL(RANDOM() * 500), -- Random employee_id
    CURRENT_DATE - INTERVAL '1 day' * FLOOR(RANDOM() * 365 * 2), -- Random order date
    CEIL(RANDOM() * 1000 + 10) -- Random total amount between $10 and $1010
FROM GENERATE_SERIES(1, 100) AS i;

INSERT INTO products (product_name, product_description, price, stock_quantity)
SELECT
    'Product_' || i,
    'Description for product ' || i,
    CEIL(RANDOM() * 100), -- Price between $1 and $101
    CEIL(RANDOM() * 1000) -- Stock quantity between 1 and 1000
FROM GENERATE_SERIES(1, 500) AS i; -- 500 products

-- 9. Insert data into order_details
INSERT INTO order_details (order_id, product_id, quantity, price)
SELECT
    CEIL(RANDOM() * 100), -- Random order_id
    CEIL(RANDOM() * 500), -- Random product_id
    CEIL(RANDOM() * 10), -- Quantity between 1 and 10
    CEIL(RANDOM() * 100 + 1) -- Price between $1 and $101
FROM GENERATE_SERIES(1, 5000) AS i; -- 5,000 order details

INSERT INTO payments (order_id, payment_date, amount, payment_method)
SELECT
    CEIL(RANDOM() * 100), -- Random order_id
    CURRENT_DATE - INTERVAL '1 day' * FLOOR(RANDOM() * 365 * 2), -- Random payment date
    CEIL(RANDOM() * 1000 + 10), -- Amount between $10 and $1010
    CASE WHEN RANDOM() < 0.5 THEN 'Credit Card' ELSE 'PayPal' END -- Random payment method
FROM GENERATE_SERIES(1, 1000) AS i; -- 1,000 payments


 \pset pager off

-- Planner settings to force certain plan types for testing
-- SET enable_seqscan = OFF;
-- SET enable_indexscan = OFF;
-- SET enable_bitmapscan = OFF;
-- SET enable_tidscan = OFF;
-- SET enable_sort = OFF;
-- SET enable_hashagg = OFF;
-- SET enable_nestloop = OFF;
-- SET enable_mergejoin = OFF;
-- SET enable_hashjoin = OFF;

select pg_store_plans_reset();
\o /dev/null

-- ===== Test Queries =====
PREPARE my_regions (text) AS SELECT region_name, region_id FROM regions WHERE region_name = $1;

EXECUTE my_regions('Region_1');
EXECUTE my_regions('Region_2');

DEALLOCATE my_regions;

\o
select calls, regexp_replace(plan, '\s*\(cost=[^)]+\)', '', 'g') AS plan from pg_store_plans where planid != 0 order by calls, plan;
select pg_store_plans_reset();
\o /dev/null


SELECT e.employee_name, e.position, b.branch_name
FROM employees e
JOIN branches b ON e.branch_id = b.branch_id
WHERE b.branch_name in ('Branch_2','Branch_3','Branch_5');

SELECT e.employee_name, e.position, b.branch_name
FROM employees e
JOIN branches b ON e.branch_id = b.branch_id
WHERE b.branch_name in ('Branch_10','Branch_1');

\g

\o
select calls, regexp_replace(plan, '\s*\(cost=[^)]+\)', '', 'g') AS plan from pg_store_plans where planid != 0 order by calls, plan;
select pg_store_plans_reset();
\o /dev/null

SELECT e.employee_name, e.position, b.branch_name
FROM employees e
JOIN branches b ON e.branch_id = b.branch_id
WHERE b.branch_name in ('Branch_10','Branch_1');
SET enable_hashjoin = OFF;
SELECT e.employee_name, e.position, b.branch_name
FROM employees e
JOIN branches b ON e.branch_id = b.branch_id
WHERE b.branch_name in ('Branch_10','Branch_1');

\g

SET enable_nestloop = OFF;

SELECT e.employee_name, e.position, b.branch_name
FROM employees e
JOIN branches b ON e.branch_id = b.branch_id
WHERE b.branch_name in ('Branch_10','Branch_1');
\g
\g
\o
select calls, regexp_replace(plan, '\s*\(cost=[^)]+\)', '', 'g') AS plan from pg_store_plans where planid != 0 order by calls, plan;
select pg_store_plans_reset();
\o /dev/null

select last_name from customers where last_name='customer128' or last_name='custom123';

select last_name from customers where last_name='customer128';

select last_name from customers where last_name>='customer1';

SET enable_seqscan = OFF;

select last_name from customers where last_name>='customer1';

select last_name from customers where last_name='custom123';

SET enable_seqscan = ON;
SET enable_indexscan = OFF;

select last_name from customers where last_name='custom123';

\o
select calls, regexp_replace(plan, '\s*\(cost=[^)]+\)', '', 'g') AS plan from pg_store_plans where planid != 0 order by calls, plan;
select pg_store_plans_reset();
\o /dev/null


SELECT employee_name, branch_id, hire_date,
    rank() OVER (PARTITION BY branch_id ORDER BY hire_date)
FROM employees;

SELECT employee_name, branch_id, hire_date,
    rank() OVER (PARTITION BY employee_name ORDER BY hire_date)
FROM employees;

SELECT employee_name, branch_id, hire_date,
    rank() OVER (PARTITION BY employee_name)
FROM employees;

\o
select calls, regexp_replace(plan, '\s*\(cost=[^)]+\)', '', 'g') AS plan from pg_store_plans where planid != 0 order by calls, plan;
select pg_store_plans_reset();
\o /dev/null

SELECT * FROM orders WHERE order_date = CURRENT_DATE -1;
SELECT * FROM orders WHERE order_date = CURRENT_DATE -2;

SELECT * FROM orders WHERE order_date = CURRENT_DATE -1 or order_date = current_date - 2;


\o
select calls, regexp_replace(plan, '\s*\(cost=[^)]+\)', '', 'g') AS plan from pg_store_plans where planid != 0 order by calls, plan;
select pg_store_plans_reset();
\o /dev/null

-- drop TABLE addresses;
-- drop TABLE payments;
-- drop TABLE order_details;
-- drop TABLE orders;
-- drop TABLE employees;
-- drop TABLE branches;
-- drop TABLE products;
-- drop TABLE customers;
-- drop TABLE countries;
-- drop TABLE regions;
