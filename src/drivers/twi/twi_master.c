/*
* twi_master.c
*
* Created: 09-Jun-19 11:20:17 AM
*  Author: TEP SOVICHEA
*/

#include "twi_master.h"

#include <stdio.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

ret_code_t tw_start(void)
{
	/* Send START condition */
	TWCR =  (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
	
	/* Wait for TWINT flag to set */
	while (!(TWCR & (1 << TWINT)));
	
#if DEBUG_LOG
	if (TW_STATUS == TW_START)
	{
		puts_P(PSTR("tw_start\n"));
	}
	else if (TW_STATUS == TW_REP_START)
	{
		puts_P(PSTR("tw_start repeated\n"));
	}
#endif

	/* Check error */
	if (TW_STATUS != TW_START && TW_STATUS != TW_REP_START)
	{
		return TW_STATUS;
	}
	
	return SUCCESS;
}


void tw_stop(void)
{
	#if DEBUG_LOG
	puts_P(PSTR("tw_stop\n"));
	#endif
	/* Send STOP condition */
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
	
	_delay_us(TW_STOP_WAIT_US);
}


ret_code_t tw_write_sla(uint8_t sla)
{
	/* Transmit slave address with read/write flag */
#if DEBUG_LOG
	printf_P(PSTR("Write SLA + R/W: 0x%02X\n"), sla);
#endif
	TWDR = sla;
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	/* Wait for TWINT flag to set */
	loop_until_bit_is_clear(TWCR, TWINT);
	if (TW_STATUS != TW_MT_SLA_ACK && TW_STATUS != TW_MR_SLA_ACK)
	{
		return TW_STATUS;
	}

	return SUCCESS;
}

void tw_get_addresses(uint8_t* addresses)
{
	for (uint8_t i = 0; i < 128; i++) {
		TWDR = i; // sla
		TWCR = (1 < TWINT) | (1 << TWEN);
		loop_until_bit_is_clear(TWCR, TWINT);
		if (TW_STATUS == TW_MT_SLA_ACK && TW_STATUS == TW_MR_SLA_ACK) {
			addresses[i] = i;
		}
	}
}

ret_code_t tw_write(uint8_t data)
{
	/* Transmit 1 byte*/
#if DEBUG_LOG
	printf_P(PSTR("Write data byte: 0x%02X\n"), data);
#endif
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	/* Wait for TWINT flag to set */
	while (!(TWCR & (1 << TWINT)));
	if (TW_STATUS != TW_MT_DATA_ACK)
	{
		return TW_STATUS;
	}
	
	return SUCCESS;
}


uint8_t tw_read(bool read_ack)
{
	if (read_ack)
	{
		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
		while (!(TWCR & (1 << TWINT)));
		if (TW_STATUS != TW_MR_DATA_ACK)
		{
			return TW_STATUS;
		}
	}
	else
	{
		TWCR = (1 << TWINT) | (1 << TWEN);
		while (!(TWCR & (1 << TWINT)));
		if (TW_STATUS != TW_MR_DATA_NACK)
		{
			return TW_STATUS;
		}
	}
	uint8_t data = TWDR;
#if DEBUG_LOG
	printf_P(PSTR("Read data byte: 0x%02X\n"), data);
#endif
	return data;
}


void tw_init(twi_freq_mode_t twi_freq_mode, bool pullup_en)
{
	DDRC  |= (1 << TW_SDA_PIN) | (1 << TW_SCL_PIN);
	if (pullup_en)
	{
#if DEBUG_LOG
		puts_P(PSTR("Enabling pull-up resistors\n"));
#endif
		PORTC |= (1 << TW_SDA_PIN) | (1 << TW_SCL_PIN);
	}
	else
	{
		PORTC &= ~((1 << TW_SDA_PIN) | (1 << TW_SCL_PIN));
	}
	DDRC  &= ~((1 << TW_SDA_PIN) | (1 << TW_SCL_PIN));
	
	switch (twi_freq_mode)
	{
		case TW_FREQ_100K:
		/* Set bit rate register 72 and prescaler to 1 resulting in
		SCL_freq = 8 MHz/(16 + 2*32*1) = 100KHz	*/
		TWBR = 32;
		break;
		
		case TW_FREQ_250K:
		/* Set bit rate register 24 and prescaler to 1 resulting in
		SCL_freq = 8 MHz/(16 + 2*8*1) = 250KHz	*/
		TWBR = 8;
		break;
		
		case TW_FREQ_400K:
		/* Set bit rate register 12 and prescaler to 1 resulting in
		SCL_freq = 8MHz/(16 + 2*2*1) = 400KHz	*/
		TWBR = 2;
		break;
		
		default: break;
	}
}


ret_code_t tw_master_transmit(uint8_t slave_addr, uint8_t* p_data, uint8_t len, bool repeat_start)
{
	ret_code_t error_code;
	
	/* Send START condition */
	error_code = tw_start();
	if (error_code != SUCCESS)
	{
		return error_code;
	}
	
	/* Send slave address with WRITE flag */
	error_code = tw_write_sla(TW_SLA_W(slave_addr));
	if (error_code != SUCCESS)
	{
		return error_code;
	}
	
	/* Send data byte in single or burst mode */
	for (int i = 0; i < len; ++i)
	{
		error_code = tw_write(p_data[i]);
		if (error_code != SUCCESS)
		{
			return error_code;
		}
	}
	
	if (!repeat_start)
	{
		/* Send STOP condition */
		tw_stop();
	}
	
	return SUCCESS;
}


ret_code_t tw_master_receive(uint8_t slave_addr, uint8_t* p_data, uint8_t len)
{
	ret_code_t error_code;
	
	/* Send START condition */
	error_code = tw_start();
	if (error_code != SUCCESS)
	{
		return error_code;
	}
	
	/* Write slave address with READ flag */
	error_code = tw_write_sla(TW_SLA_R(slave_addr));
	if (error_code != SUCCESS)
	{
		return error_code;
	}
	
	/* Read single or multiple data byte and send ack */
	for (int i = 0; i < len-1; ++i)
	{
		p_data[i] = tw_read(TW_READ_ACK);
	}
	p_data[len-1] = tw_read(TW_READ_NACK);
	
	/* Send STOP condition */
	tw_stop();
	
	return SUCCESS;
}