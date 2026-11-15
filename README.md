# mod-heartstone-no-cd

Devuelve/configura el enfriamiento de la Piedra de Hogar (spell 8690) al castearla.
No requiere SQL.

## Instalacion
1. Copia la carpeta a `modules/`, re-ejecuta CMake y recompila.
2. Copia `mod-heartstone-no-cd.conf.dist` como `mod-heartstone-no-cd.conf` en `configs/modules/`.

## Opciones
| Opcion | Default | Descripcion |
|---|---|---|
| `HeartstoneNoCd.Enable` | 1 | Activa/desactiva el modulo |
| `HeartstoneNoCd.Cooldown` | 0 | Segundos de CD (0 = sin CD) |
| `HeartstoneNoCd.Bots.Enable` | 0 | Aplicar tambien a playerbots |
