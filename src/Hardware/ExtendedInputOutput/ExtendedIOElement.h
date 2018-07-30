#pragma once

#include <Helpers/LinkedList.h>
#include <Def/Def.hpp>

/**
 * @brief   A class for extended IO elements.
 *
 *          The limited number of IO pins of the Arduino can be extended by
 *          adding multiplexers, shift registers, IO expanders, etc.
 *          ExtendedIOElement serves as a base class for all these expanders.
 *
 *          The pins of each extended IO element are mapped to a pin number
 *          greater than the greatest Arduino pin number.
 *          You can supply this pin number to the IO functions in the ExtIO
 *          namespace.
 *          If the pin number corresponds to an actual Arduino pin,
 *          the default Arduino IO function (digitalRead, digitalWrite, ...)
 *          will be called.
 *          If the pin is not an Arduino pin, it is an extended IO pin number,
 *          so the extended IO element that this pin belongs to is looked up,
 *          and the IO function for this element is executed with the correct
 *          pin number.
 *
 *          For example:
 *          Imagine an Arduino with 20 pins (e.g. the Arduino UNO).
 *          Pins 0 - 19 will correspond to the Arduino pins, and
 *          `ExtIO::digitalRead(pin)` will have the exact same effect as
 *          the standard `digitalRead(pin)` function.
 *          Now, we'll add two 8-channel analog multiplexers, let's call them
 *          `mux1` and `mux2`.
 *          The first pin (pin 0) of `mux1` will be extended IO pin number 20,
 *          the last pin (pin 7) of `mux1` will be extended IO pin number 27,
 *          etc.
 *          The first pin of `mux2` will be extended IO pin number 28, you get
 *          the idea.
 *          If you now call `ExtIO::digitalRead(mux1.#pin (7))` or
 *          `ExtIO::digitalRead(27)`, both will be
 *          translated to `mux1.digitalRead(7)`.
 *
 *          The number of extended IO elements is limited only by the size of
 *          #pin_t. However, looking up the extended IO element for a given
 *          extended IO pin number uses linear search, so that might add
 *          some noticable overhead for large pin numbers.
 */
class ExtendedIOElement {
  protected:
    ExtendedIOElement(pin_t length);

  public:
    virtual ~ExtendedIOElement();

    virtual void pinMode(pin_t pin, uint8_t mode) = 0;
    virtual void digitalWrite(pin_t pin, uint8_t mode) = 0;
    virtual int digitalRead(pin_t pin) = 0;
    virtual analog_t analogRead(pin_t pin) = 0;
    virtual void analogWrite(pin_t pin, analog_t val) = 0;

    /**
     * @brief   Initialize the extended IO element.
     */
    virtual void begin() = 0;

    static void beginAll();

    /**
     * @brief   Update the extended IO element:
     *          write the internal state to the physical outputs,
     *          or read the physical state into the input buffers.
     */
    virtual void update() = 0;

    /**
     * @brief   Get the extended IO pin number of a given physical pin of this
     *          extended IO element.
     * @param   pin
     *          The zero-based physical pin number of this IO element.
     * @return  The global, unique extended IO pin number for the given pin.
     */
    pin_t pin(pin_t pin);
    /**
     * @brief   Get the greatest global extended IO pin number that belongs to
     *          this extended IO element.
     */
    pin_t getEnd();
    /**
     * @brief   Get the smallest global extended IO pin number that belongs to
     *          this extended IO element.
     */
    pin_t getStart();

    ExtendedIOElement *getNext();
    static ExtendedIOElement *getFirst();

  private:
    const pin_t length;
    const pin_t start;
    const pin_t end;
    static pin_t offset;

    ExtendedIOElement *next = nullptr, *previous = nullptr;
    static ExtendedIOElement *last;
    static ExtendedIOElement *first;
    template <class Node>
    friend void LinkedList::append(Node *, Node *&, Node *&);
    template <class Node>
    friend void LinkedList::remove(Node *, Node *&, Node *&);
};