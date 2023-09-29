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

static err_t uart0_open() {
	uart_write_reg(IER, 0x00);

	uint8_t lcr = uart_read_reg(LCR);
	uart_write_reg(LCR, lcr | (1 << 7));
	uart_write_reg(DLL, 0x03);
	uart_write_reg(DLM, 0x00);

	lcr = 0;
	uart_write_reg(LCR, lcr | (3 << 0));
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
static err_t uart_input(size_t size)
{
    char buffer[2];
    buffer[0]=uart_read_reg(RHR);
    buffer[1]='\0';
    /* After the uart device receives the data, it generates an interrupt, calls this callback function, and then sends the received semaphore. */
    uart0_puts(0, buffer, 1);
    sem_release((uint16_t)rx_sem);
    return E_DEV_OK;
}

err_t waitRxData(int timeout) {
    return sem_take(rx_sem, timeout);
}

void setConsole(deviceCB_t *drv) {
    console = drv;
}





err_t console_init() {

    rx_sem = createSem(1,1,PRIO);
    if (rx_sem<0) {
        return E_CREATE_FAIL;
    }

    serial = device_create(Device_Class_Char,CONSOLE_PORT_NAME);
    //set up deviceCB_t data
    serial->init = NULL;
    serial->open = uart0_open;
    serial->close = NULL;
    serial->read = NULL;
    serial->write = uart0_puts;
    serial->control = NULL;
    device_set_rxReady(serial, uart_input);   

    err_t regResult = device_register(serial, CONSOLE_PORT_NAME, 
                      FLAG_RDWR|FLAG_DEACTIVATE);
    if (regResult != E_DEV_OK) {
        return E_CREATE_FAIL;
    }

    device_open(serial, FLAG_INT_RX);
    setConsole(serial);
    return E_DEV_OK;
}



