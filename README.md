# SBB Kletterhalle

Nutzerinterface für Zugangskontrollsystem.

## Ablaufdiagramme

Es gibt zwei parallel arbeitende Zustandsmaschinen.
(Diagramme bearbeitet mit yEd).

### Door State Machine

Diese Automat behandelt Ereignisse an der Tuer. Das Freigabesignal `VALID_TICKET` erhaehlt er von der Scanner State Machine.
Der Verbrauch / Verfall eines Tickets wird per `CONTINUE` Ereignis an die Scanner State Machine gemeldet.

![Door State Machine](state-machine-door.png?raw=true)


### Scanner State Machine

![Scanner State Machine](state-machine-scanner.png?raw=true)

