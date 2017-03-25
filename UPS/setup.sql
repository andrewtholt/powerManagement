
CREATE TABLE line_voltage_log (
    ID INTEGER PRIMARY KEY,
    VAC varchar(6),
    Timestamp DATETIME DEFAULT CURRENT_TIMESTAMP
);
