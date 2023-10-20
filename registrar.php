<?php

$usuario = $_POST['txt_usuario'];
$nombre = $_POST['txt_nombre'];
$edad = $_POST['int_edad'];

try {

    $conexion = new PDO("mysql:host=localhost;port=3306;dbname=prueba_db", "root", "h4d33sc22");
    $conexion->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $conexion->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);

    $pdo = $conexion->prepare('INSERT INTO usuarios(usuario, nombre, edad) VALUES(?, ?, ?)');
    $pdo->bindParam(1, $usuario);
    $pdo->bindParam(2, $nombre);
    $pdo->bindParam(3, $edad);
    $pdo->execute() or die(print($pdo->errorInfo()));

    echo json_encode('true');

} catch(PDOException $error) {
    echo $error->getMessage();
    die();
}