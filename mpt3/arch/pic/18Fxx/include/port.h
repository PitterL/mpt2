/**
 * \file
 *
 * \brief Port related support
 *
 (c) 2020 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

#ifndef PORT_INCLUDED
#define PORT_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <pin_manager.h>
#include <xc.h>
    
/**
 * \brief Set port pin pull mode
 *
 * Configure pin to pull up, down or disable pull mode, supported pull modes are defined by device used
 *
 * \param[in] pin       The pin number within port
 * \param[in] pull_mode Pin pull mode
 */
typedef enum port_pull_mode {
	PORT_PULL_OFF,
	PORT_PULL_UP,
} PORT_PULL_MODE_t;

static inline void PORTA_set_pin_pull_mode(const uint8_t pin, const PORT_PULL_MODE_t pull_mode)
{
	const uint8_t mask = (uint8_t)(1 << pin);

	if (pull_mode == PORT_PULL_UP) {
		WPUA |= mask;
	} else if (pull_mode == PORT_PULL_OFF) {
		WPUA &= ~mask;
	}
}

/**
 * \brief Set port pin inverted mode
 *
 * Configure pin invert I/O or not
 *
 * \param[in] pin       The pin number within port
 * \param[in] inverted  Pin inverted mode
 */
static inline void PORTA_pin_set_inverted(const uint8_t pin, const bool inverted)
{
    // Not supported
}

/**
 * \brief Set port pin input/sense configuration
 *
 * Enable/disable digital input buffer and pin change interrupt,
 * select pin interrupt edge/level sensing mode
 *
 * \param[in] pin pin number within port
 * \param[in] isc PORT_ISC_INTDISABLE_gc    = Interrupt disabled but input buffer enabled
 *                PORT_ISC_BOTHEDGES_gc     = Sense Both Edges
 *                PORT_ISC_RISING_gc        = Sense Rising Edge
 *                PORT_ISC_FALLING_gc       = Sense Falling Edge
 *                PORT_ISC_INPUT_DISABLE_gc = Digital Input Buffer disabled
 *                PORT_ISC_LEVEL_gc         = Sense low Level
 *
 */
/* Input/Sense Configuration select */
typedef enum PORT_ISC_enum
{
    PORT_ISC_INTDISABLE_gc = (0x00<<0),  /* Interrupt disabled but input buffer enabled */
    PORT_ISC_BOTHEDGES_gc = (0x01<<0),  /* Sense Both Edges */
    PORT_ISC_RISING_gc = (0x02<<0),  /* Sense Rising Edge */
    PORT_ISC_FALLING_gc = (0x03<<0),  /* Sense Falling Edge */
    PORT_ISC_INPUT_DISABLE_gc = (0x04<<0),  /* Digital Input Buffer disabled */
    PORT_ISC_LEVEL_gc = (0x05<<0),  /* Sense low Level */
} PORT_ISC_t;

static inline void PORTA_pin_set_isc(const uint8_t pin, const PORT_ISC_t isc)
{
    const uint8_t mask = (uint8_t)(1 << pin);

	switch(isc) {
        case PORT_ISC_INTDISABLE_gc:
            IOCAF &= ~mask;
            IOCAN &= ~mask;
            IOCAP &= ~mask;
            ANSELA &= ~mask;
            break;
        case PORT_ISC_BOTHEDGES_gc:
            IOCAF &= ~mask;
            IOCAN |= mask;
            IOCAP |= mask;
            break;
        case PORT_ISC_RISING_gc:
            IOCAF &= ~mask;
            IOCAN &= ~mask;
            IOCAP |= mask;
            break;
        case PORT_ISC_FALLING_gc:
            IOCAF &= ~mask;
            IOCAN |= mask;
            IOCAP &= ~mask;
            break;
        case PORT_ISC_INPUT_DISABLE_gc:
            ANSELA |= mask;
            break;
        case PORT_ISC_LEVEL_gc:
            IOCAF |= mask;
            IOCAN |= mask;
            IOCAP &= ~mask;
            break;
        default:
            ;
    }
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OUT_OD = Data direction out with Open Drain
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
typedef enum port_dir {
	PORT_DIR_IN,
	PORT_DIR_OUT,
    PORT_DIR_OUT_OD,
	PORT_DIR_OFF,
} PORT_DIR_t;

static inline void PORTA_set_port_dir(const uint8_t mask, const PORT_DIR_t dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		TRISA |= mask;
		break;
    case PORT_DIR_OUT_OD:
        ODCONA |= mask;
		TRISA &= ~mask;
        break;
	case PORT_DIR_OUT:
        ODCONA &= ~mask;
		TRISA &= ~mask;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port pin data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number within port
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OUT_OD = Data direction out with Open Drain
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTA_set_pin_dir(const uint8_t pin, const PORT_DIR_t dir)
{
	PORTA_set_port_dir((uint8_t)(1 << pin), dir);
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void PORTA_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		LATA |= mask;
	} else {
		LATA &= ~mask;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number within port
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void PORTA_set_pin_level(const uint8_t pin, const bool level)
{
	PORTA_set_port_level((uint8_t)(1 << pin), level);
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void PORTA_toggle_port_level(const uint8_t mask)
{
	LATA ^= mask;
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number within port
 */
static inline void PORTA_toggle_pin_level(const uint8_t pin)
{
	PORTA_toggle_port_level((uint8_t)(1 << pin));
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 */
static inline uint8_t PORTA_get_port_level()
{
	return PORTA;
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 */
static inline bool PORTA_get_pin_level(const uint8_t pin)
{
	return PORTA & (uint8_t)(1 << pin);
}

/**
 * \brief Write value to Port
 *
 * Write directly to the port OUT register
 *
 * \param[in] value Value to write to the port register
 */
static inline void PORTA_write_port(const uint8_t value)
{
	LATA = value;
}

/**
 * \brief Set port pin pull mode
 *
 * Configure pin to pull up, down or disable pull mode, supported pull modes are defined by device used
 *
 * \param[in] pin       The pin number within port
 * \param[in] pull_mode Pin pull mode
 */
static inline void PORTB_set_pin_pull_mode(const uint8_t pin, const PORT_PULL_MODE_t pull_mode)
{
	const uint8_t mask = (uint8_t)(1 << pin);

	if (pull_mode == PORT_PULL_UP) {
		WPUB |= mask;
	} else if (pull_mode == PORT_PULL_OFF) {
		WPUB &= ~mask;
	}
}

/**
 * \brief Set port pin inverted mode
 *
 * Configure pin invert I/O or not
 *
 * \param[in] pin       The pin number within port
 * \param[in] inverted  Pin inverted mode
 */
static inline void PORTB_pin_set_inverted(const uint8_t pin, const bool inverted)
{
	// Not supported
}

/**
 * \brief Set port pin input/sense configuration
 *
 * Enable/disable digital input buffer and pin change interrupt,
 * select pin interrupt edge/level sensing mode
 *
 * \param[in] pin pin number within port
 * \param[in] isc PORT_ISC_INTDISABLE_gc    = Interrupt disabled but input buffer enabled
 *                PORT_ISC_BOTHEDGES_gc     = Sense Both Edges
 *                PORT_ISC_RISING_gc        = Sense Rising Edge
 *                PORT_ISC_FALLING_gc       = Sense Falling Edge
 *                PORT_ISC_INPUT_DISABLE_gc = Digital Input Buffer disabled
 *                PORT_ISC_LEVEL_gc         = Sense low Level
 *
 */
static inline void PORTB_pin_set_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	const uint8_t mask = (uint8_t)(1 << pin);

	switch(isc) {
        case PORT_ISC_INTDISABLE_gc:
            IOCBF &= ~mask;
            IOCBN &= ~mask;
            IOCBP &= ~mask;
            ANSELB &= ~mask;
            break;
        case PORT_ISC_BOTHEDGES_gc:
            IOCBF &= ~mask;
            IOCBN |= mask;
            IOCBP |= mask;
            break;
        case PORT_ISC_RISING_gc:
            IOCBF &= ~mask;
            IOCBN &= ~mask;
            IOCBP |= mask;
            break;
        case PORT_ISC_FALLING_gc:
            IOCBF &= ~mask;
            IOCBN |= mask;
            IOCBP &= ~mask;
            break;
        case PORT_ISC_INPUT_DISABLE_gc:
            ANSELB |= mask;
            break;
        case PORT_ISC_LEVEL_gc:
            IOCBF |= mask;
            IOCBN |= mask;
            IOCBP &= ~mask;
            break;
        default:
            ;
    }
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OUT_OD = Data direction out with Open Drain
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTB_set_port_dir(const uint8_t mask, const PORT_DIR_t dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		TRISB |= mask;
		break;
    case PORT_DIR_OUT_OD:
        ODCONB |= mask;
		TRISB &= ~mask;
        break;
	case PORT_DIR_OUT:
        ODCONB &= ~mask;
		TRISB &= ~mask;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port pin data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number within port
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OUT_OD = Data direction out with Open Drain
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTB_set_pin_dir(const uint8_t pin, const PORT_DIR_t dir)
{
	PORTB_set_port_dir((uint8_t)(1 << pin), dir);
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void PORTB_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		LATB |= mask;
	} else {
		LATB &= ~mask;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number within port
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void PORTB_set_pin_level(const uint8_t pin, const bool level)
{
	PORTB_set_port_level((uint8_t)(1 << pin), level);
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void PORTB_toggle_port_level(const uint8_t mask)
{
	LATB ^= mask;
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number within port
 */
static inline void PORTB_toggle_pin_level(const uint8_t pin)
{
	PORTB_toggle_port_level((uint8_t)(1 << pin));
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 */
static inline uint8_t PORTB_get_port_level()
{
	return PORTB;
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 */
static inline bool PORTB_get_pin_level(const uint8_t pin)
{
	return PORTB & ((uint8_t)1 << pin);
}

/**
 * \brief Write value to Port
 *
 * Write directly to the port OUT register
 *
 * \param[in] value Value to write to the port register
 */
static inline void PORTB_write_port(const uint8_t value)
{
	LATB = value;
}

/**
 * \brief Set port pin pull mode
 *
 * Configure pin to pull up, down or disable pull mode, supported pull modes are defined by device used
 *
 * \param[in] pin       The pin number within port
 * \param[in] pull_mode Pin pull mode
 */
static inline void PORTC_set_pin_pull_mode(const uint8_t pin, const PORT_PULL_MODE_t pull_mode)
{
	const uint8_t mask = (uint8_t)(1 << pin);

	if (pull_mode == PORT_PULL_UP) {
		WPUC |= mask;
	} else if (pull_mode == PORT_PULL_OFF) {
		WPUC &= ~mask;
	}
}

/**
 * \brief Set port pin inverted mode
 *
 * Configure pin invert I/O or not
 *
 * \param[in] pin       The pin number within port
 * \param[in] inverted  Pin inverted mode
 */
static inline void PORTC_pin_set_inverted(const uint8_t pin, const bool inverted)
{
	// Not supported
}

/**
 * \brief Set port pin input/sense configuration
 *
 * Enable/disable digital input buffer and pin change interrupt,
 * select pin interrupt edge/level sensing mode
 *
 * \param[in] pin pin number within port
 * \param[in] isc PORT_ISC_INTDISABLE_gc    = Interrupt disabled but input buffer enabled
 *                PORT_ISC_BOTHEDGES_gc     = Sense Both Edges
 *                PORT_ISC_RISING_gc        = Sense Rising Edge
 *                PORT_ISC_FALLING_gc       = Sense Falling Edge
 *                PORT_ISC_INPUT_DISABLE_gc = Digital Input Buffer disabled
 *                PORT_ISC_LEVEL_gc         = Sense low Level
 *
 */
static inline void PORTC_pin_set_isc(const uint8_t pin, const PORT_ISC_t isc)
{
	const uint8_t mask = (uint8_t)(1 << pin);

	switch(isc) {
        case PORT_ISC_INTDISABLE_gc:
            IOCCF &= ~mask;
            IOCCN &= ~mask;
            IOCCP &= ~mask;
            ANSELC &= ~mask;
            break;
        case PORT_ISC_BOTHEDGES_gc:
            IOCCF &= ~mask;
            IOCCN |= mask;
            IOCCP |= mask;
            break;
        case PORT_ISC_RISING_gc:
            IOCCF &= ~mask;
            IOCCN &= ~mask;
            IOCCP |= mask;
            break;
        case PORT_ISC_FALLING_gc:
            IOCCF &= ~mask;
            IOCCN |= mask;
            IOCCP &= ~mask;
            break;
        case PORT_ISC_INPUT_DISABLE_gc:
            ANSELC |= mask;
            break;
        case PORT_ISC_LEVEL_gc:
            IOCCF |= mask;
            IOCCN |= mask;
            IOCCP &= ~mask;
            break;
        default:
            ;
    }
}

/**
 * \brief Set port data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] mask      Bit mask where 1 means apply direction setting to the
 *                      corresponding pin
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OUT_OD = Data direction out with Open Drain
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTC_set_port_dir(const uint8_t mask, const PORT_DIR_t dir)
{
	switch (dir) {
	case PORT_DIR_IN:
		TRISC |= mask;
		break;
    case PORT_DIR_OUT_OD:
        ODCONC |= mask;
		TRISC &= ~mask;
        break;
	case PORT_DIR_OUT:
        ODCONC &= ~mask;
		TRISC &= ~mask;
		break;
	default:
		break;
	}
}

/**
 * \brief Set port pin data direction
 *
 * Select if the pin data direction is input, output or disabled.
 * If disabled state is not possible, this function throws an assert.
 *
 * \param[in] pin       The pin number within port
 * \param[in] dir       PORT_DIR_IN  = Data direction in
 *                      PORT_DIR_OUT = Data direction out
 *                      PORT_DIR_OUT_OD = Data direction out with Open Drain
 *                      PORT_DIR_OFF = Disables the pin
 *                      (low power state)
 */
static inline void PORTC_set_pin_dir(const uint8_t pin, const PORT_DIR_t dir)
{
	PORTC_set_port_dir((uint8_t)(1 << pin), dir);
}

/**
 * \brief Set port level
 *
 * Sets output level on the pins defined by the bit mask
 *
 * \param[in] mask  Bit mask where 1 means apply port level to the corresponding
 *                  pin
 * \param[in] level true  = Pin levels set to "high" state
 *                  false = Pin levels set to "low" state
 */
static inline void PORTC_set_port_level(const uint8_t mask, const bool level)
{
	if (level == true) {
		LATC |= mask;
	} else {
		LATC &= ~mask;
	}
}

/**
 * \brief Set port level
 *
 * Sets output level on a pin
 *
 * \param[in] pin       The pin number within port
 * \param[in] level true  = Pin level set to "high" state
 *                  false = Pin level set to "low" state
 */
static inline void PORTC_set_pin_level(const uint8_t pin, const bool level)
{
	PORTC_set_port_level((uint8_t)(1 << pin), level);
}

/**
 * \brief Toggle out level on pins
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] mask  Bit mask where 1 means toggle pin level to the corresponding
 *                  pin
 */
static inline void PORTC_toggle_port_level(const uint8_t mask)
{
	LATC ^= mask;
}

/**
 * \brief Toggle output level on pin
 *
 * Toggle the pin levels on pins defined by bit mask
 *
 * \param[in] pin       The pin number within port
 */
static inline void PORTC_toggle_pin_level(const uint8_t pin)
{
	PORTC_toggle_port_level((uint8_t)(1 << pin));
}

/**
 * \brief Get input level on pins
 *
 * Read the input level on pins connected to a port
 *
 */
static inline uint8_t PORTC_get_port_level()
{
	return PORTC;
}

/**
 * \brief Get level on pin
 *
 * Reads the level on pins connected to a port
 */
static inline bool PORTC_get_pin_level(const uint8_t pin)
{
	return PORTC & (1 << pin);
}

/**
 * \brief Write value to Port
 *
 * Write directly to the port OUT register
 *
 * \param[in] value Value to write to the port register
 */
static inline void PORTC_write_port(const uint8_t value)
{
	LATC = value;
}

#ifdef __cplusplus
}
#endif

#endif /* PORT_INCLUDED */
