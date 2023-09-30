// uart0 in Qemu driver
// interrupt rx and tx polling
// so driver must implement 
// 1. rxReady for rx interrupt
// 2. write for tx polling send a string
// 3. open 
// 4. no read, close, init

#include "os.h"

#define CONSOLE_PORT_NAME       "uart0"
/* Semaphore used to receive messages */
static err_t rx_sem;
deviceCB_t *serial, *console;

static err_t uart0_init() {
	uart_write_reg(IER, 0x00);

	uint8_t lcr = uart_read_reg(LCR);
	uart_write_reg(LCR, lcr | (1 << 7));
	uart_write_reg(DLL, 0x03);
	uart_write_reg(DLM, 0x00);

	lcr = 0;
	uart_write_reg(LCR, lcr | (3 << 0));

    /*
	 * enable receive interrupts.
	 */
	uint8_t ier = uart_read_reg(IER);
	uart_write_reg(IER, ier | (1 << 0));    
    return E_DEV_OK;
}


static int uart0_putc(char ch)
{
	while ((uart_read_reg(LSR) & LSR_TX_IDLE) == 0);
	return uart_write_reg(THR, ch);
}

/* transmition data for driver write function*/
static size_t uart0_puts(int pos, const void *buffer, size_t size) {
    char *s = (char *)buffer;
	while (*s) {
		uart0_putc(*s++);
	}
    return size;
}


/* Receive data callback function rxReady */
static err_t uart0_getc(size_t size)
{
    char rxChar = -1;
    if (uart_read_reg(LSR) & LSR_RX_READY)
		rxChar = uart_read_reg(RHR);
    sem_release((uint16_t)rx_sem);
    return rxChar;
}

err_t waitRxData(int timeout) {
    return sem_take(rx_sem, timeout);
}

void setConsole(deviceCB_t *drv) {
    console = drv;
}


/*
 * handle a uart interrupt, raised because input has arrived, called from exteralInterrrupt().
 */
void uart0_isr(void)
{
    if (uart_read_reg(LSR) & LSR_RX_READY) {
        char rxChar = (char)serial->rxReady(1);
        //process rx print or store in a buffer
        //we can store rxChar in a global buffer
        //then access by pos offset
        //here we only send it back no process anymore
        char buffer[2]={0};
        buffer[0] = rxChar;
        serial->write(0,buffer, 1); 
    } else {
        char msg[]="only suppose rx interrupt\n";
        serial->write(0, msg, 25);
    }
}

err_t console_init() {

    rx_sem = createSem(1,1,PRIO);
    if (rx_sem<0) {
        return E_CREATE_FAIL;
    }

    serial = device_create(Device_Class_Char,CONSOLE_PORT_NAME);
    //set up deviceCB_t data
    serial->init = uart0_init;
    serial->open = NULL;
    serial->close = NULL;
    serial->read = NULL;
    serial->write = uart0_puts;
    serial->control = NULL;
    device_set_rxReady(serial, uart0_getc);   

    err_t regResult = device_register(serial, CONSOLE_PORT_NAME, 
                      FLAG_RDWR|FLAG_DEACTIVATE);
    if (regResult != E_DEV_OK) {
        return E_CREATE_FAIL;
    }

    device_open(serial, FLAG_INT_RX);
    setConsole(serial);
    	/* register ISR to ISRTbl*/
	isrRegister(UART0_IRQ, uart0_isr);
    return E_DEV_OK;
}