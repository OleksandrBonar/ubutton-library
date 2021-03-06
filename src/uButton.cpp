#include <uButton.h>

uButton::uButton(int pin): uButton(pin, INPUT_PULLUP) {}

uButton::uButton(int pin, int mode)
{
	pinNum = pin;
	pinMod = mode;
}

void uButton::begin(bool is_on)
{
	debounceTime = 0;
	count = 0;
	countMode = COUNT_FALLING;

	if (pinNum >= 0) {
		pinMode(pinNum, pinMod);
	}
	
	setState(is_on ? getOnValue() : getOffValue());

	previousSteadyState = getState();
	lastSteadyState = previousSteadyState;
	lastFlickerableState = previousSteadyState;

	lastDebounceTime = 0;
}

int uButton::getOnValue(void)
{
	return pinMod == INPUT ? HIGH : LOW;
}

int uButton::getOffValue(void)
{
	return pinMod == INPUT ? LOW : HIGH;
}

void uButton::setDebounceTime(unsigned long time)
{
	debounceTime = time;
}

void uButton::setOn(void)
{
	setState(getOnValue());
}

void uButton::setOff(void)
{
	setState(getOffValue());
}

void uButton::setState(int state)
{
	if (pinNum >= 0) {
		if (pinMod == OUTPUT) {
			virtualState = state;
			digitalWrite(pinNum, state);
		}
	} else {
		virtualState = state;
	}
}

int uButton::getState(void)
{
	return pinNum >= 0 && pinMod != OUTPUT ? digitalRead(pinNum) : virtualState;
}

int uButton::getStateLast(void)
{
	return lastSteadyState;
}

int uButton::getStatePrevious(void)
{
	return previousSteadyState;
}

bool uButton::isOn(void)
{
	return getStateLast() == getOnValue();
}

bool uButton::isOff(void)
{
	return getStateLast() == getOffValue();
}

bool uButton::isPressed(void)
{
	return getStatePrevious() == getOffValue() && getStateLast() == getOnValue();
}

bool uButton::isReleased(void)
{
	return getStatePrevious() == getOnValue() && getStateLast() == getOffValue();
}

bool uButton::isChanged(void)
{
  return getStatePrevious() != getStateLast();
}

void uButton::setCountMode(int mode)
{
	countMode = mode;
}

unsigned long uButton::getCount(void)
{
	return count;
}

void uButton::resetCount(void)
{
	count = 0;
}

void uButton::loop(void)
{
	int currentState = getState();
	unsigned long currentTime = millis();

	// If the switch/button changed, due to noise or pressing:
	if (currentState != lastFlickerableState) {
		// reset the debouncing timer
		lastDebounceTime = currentTime;
		// save the the last flickerable state
		lastFlickerableState = currentState;
	}

	if ((currentTime - lastDebounceTime) >= debounceTime) {
		// whatever the reading is at, it's been there for longer than the debounce
		// delay, so take it as the actual current state:

		// save the the steady state
		previousSteadyState = lastSteadyState;
		lastSteadyState = currentState;
	}

	if (isChanged()) {
		if (countMode == COUNT_BOTH) {
			count++;
		} else if (countMode == COUNT_FALLING) {
			if (isPressed()) {
				count++;
			}
		} else if (countMode == COUNT_RISING) {
			if (isReleased()) {
				count++;
			}
		}
	}
}
