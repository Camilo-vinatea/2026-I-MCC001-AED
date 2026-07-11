DROP TABLE IF EXISTS clientes_demo;

CREATE TEMP TABLE clientes_demo (
    id SERIAL PRIMARY KEY,
    nombre TEXT,
    email TEXT,
    edad INT,
    ubicacion POINT
);


-- Crear datos de prueba
INSERT INTO clientes_demo(nombre,email,edad,ubicacion)
SELECT
    'Cliente ' || i,
    'cliente' || i || '@mail.com',
    (random()*70)::INT,
    POINT(random()*100, random()*100)
FROM generate_series(1,100000) i;


-- Crear índices
CREATE INDEX idx_edad_btree
ON clientes_demo USING BTREE(edad);

CREATE INDEX idx_email_hash
ON clientes_demo USING HASH(email);

CREATE INDEX idx_ubicacion_gist
ON clientes_demo USING GIST(ubicacion);


ANALYZE clientes_demo;


-- Forzar uso de índices para la demostración
SET enable_seqscan = OFF;


-- ==================================================
-- COMPARACIÓN B-TREE
-- ==================================================

EXPLAIN (ANALYZE, FORMAT TEXT)
SELECT *
FROM clientes_demo
WHERE edad BETWEEN 30 AND 40;


-- ==================================================
-- COMPARACIÓN HASH
-- ==================================================

EXPLAIN (ANALYZE, FORMAT TEXT)
SELECT *
FROM clientes_demo
WHERE email='cliente50000@mail.com';


-- ==================================================
-- COMPARACIÓN GiST
-- ==================================================

EXPLAIN (ANALYZE, FORMAT TEXT)
SELECT *
FROM clientes_demo
WHERE ubicacion <@ circle '(50,50),10';