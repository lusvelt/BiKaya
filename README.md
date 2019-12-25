# BiKaya OS - Progetto SO A.A. 2019/20

BiKaya è un sistema operativo realizzato a scopo didattico per gli emulatori
uARM e uMPS2.

## Contributing

La compilazione del progetto è basata su Make e utlizza, oltre al Makefile contenuto
nella root directory altri 3 Makefiles siti nella cartella `makefiles`:
- `base.mk`: contiene tutte le regole per la creazione dei file eseguibili specifici
per ciascuna architettura. In particolare, compila (senza doverli specificare esplicitamente) tutti i file ```*.c``` presenti all'interno della directory `src`. Per compilare per una specifica architettura è necessario che siano definite le seguenti variabili: 
  * `ARCH`: `uarm` o `umps`
  * `ARCH_PREFIX`: il prefisso del nome del pacchetto contenente la toolchain (` arm-none-eabi-` o `mipsel-linux-gnu-`)
  * `ARCH_OBJCPY`: il nome del tool per generare il file eseguibile dall'emulatore a partire
dal `.elf`
  * `ARCH_OBJS`: i nomi dei file addizionali da compilare insieme ai sorgenti (e.g. `lib$(ARCH).o`)
  * `ARCH_CFLAGS`: flags aggiuntivi da passare a `gcc` (fra cui `-DTARGET_$(ARCH)` per definire le macro necessarie a utilizzare comportamenti diversi a seconda dell'architettura)
  * `ARCH_LDSCRIPT`: linker script specifico dell'architettura
- `uarm.mk`: definisce le variabili di cui sopra per l'emulatore uARM e include il file `base.mk`
- `umps.mk`: definisce le variabili di cui sopra per l'emulatore uMPS e include il file `base.mk`

Il `Makefile` di root espone quindi 4 regole per compilare il progetto:
- `make uarm`: compila il progetto per l'emulatore uARM
- `make [umps|umps2]`: compila il progetto per l'emulatore uMPS
- `make`: compila per entrambe le architetture
- `make clean`: rimuove tutti i file di output dalla cartella `out`