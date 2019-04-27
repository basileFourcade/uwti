#ifndef __BUTTON_H__
#define __BUTTON_H__

/*
 Gestion de l'interrupteur
 int b = checkButton();
 if (b == 1) clickEvent();
 if (b == 2) doubleClickEvent();
 if (b == 3) holdEvent();
 if (b == 4) longHoldEvent();
 }
 */

// Button
#define NB_BUTTONS				2
#define LED_BUTTON_MODE_PIN		4
#define LED_TOUCH_PANEL_PIN 	A5

// Button timing variables
#define DEBOUNCE		10 		// ms debounce period to prevent flickering when pressing or releasing the button
#define DCGAP			200 	// max ms between clicks for a double click event
#define HOLDTIME		1500 	// ms hold period: how long to wait for press+hold event
#define LONGHOLDTIME 	4000	// ms long hold period: how long to wait for press+hold event

// Other button variables
boolean buttonVal[NB_BUTTONS] = { HIGH }; 	// value read from button
boolean buttonLast[NB_BUTTONS] = { HIGH }; 	// buffered value of the button's previous state
boolean DCwaiting[NB_BUTTONS] = { false }; 	// whether we're waiting for a double click (down)
boolean DConUp[NB_BUTTONS] = { false }; 	// whether to register a double click on next release, or whether to wait and click
boolean singleOK[NB_BUTTONS] = { true }; 	// whether it's OK to do a single click
long downTime[NB_BUTTONS] = { -1 }; 		// time the button was pressed down
long upTime[NB_BUTTONS] = { -1 }; 			// time the button was released
boolean ignoreUp[NB_BUTTONS] = { false }; 	// whether to ignore the button release because the click+hold was triggered
boolean waitForUp[NB_BUTTONS] = { false }; 	// when held, whether to wait for the up event
boolean holdEventPast[NB_BUTTONS] = { false }; // whether or not the hold event happened already
boolean longHoldEventPast[NB_BUTTONS] = { false }; // whether or not the long hold event happened already

/*
 Run checkButton() to retrieve a button event:
 Click
 Double-Click
 Hold
 Long Hold
 */
enum button_event_t
{
	NO_EVENT = 0, CLICK = 1, DOUBLE_CLICK = 2, HOLD = 3, LONG_HOLD = 4,
};

button_event_t checkButton(uint8_t pin)
{
	button_event_t event = NO_EVENT;
	uint8_t idx = 0;

	// switch idx depending on pin
	if (pin == LED_TOUCH_PANEL_PIN)
	{
		idx = 0;
	}
	else // LED_BUTTON_MODE_PIN
	{
		idx = 1;
	}

	// Read the state of the button
	buttonVal[idx] = digitalRead(pin);

	// Button pressed down
	if (buttonVal[idx] == LOW && buttonLast[idx] == HIGH && (millis() - upTime[idx]) > DEBOUNCE)
	{
		downTime[idx] = millis();
		ignoreUp[idx] = false;
		waitForUp[idx] = false;
		singleOK[idx] = true;
		holdEventPast[idx] = false;
		longHoldEventPast[idx] = false;
		if ((millis() - upTime[idx]) < DCGAP && DConUp[idx] == false && DCwaiting[idx] == true)
			DConUp[idx] = true;
		else
			DConUp[idx] = false;
		DCwaiting[idx] = false;
	}
// Button released
	else if (buttonVal[idx] == HIGH && buttonLast[idx] == LOW && (millis() - downTime[idx]) > DEBOUNCE)
	{
		if (not ignoreUp[idx])
		{
			upTime[idx] = millis();
			if (DConUp[idx] == false)
			{
				DCwaiting[idx] = true;
			}
			else
			{
				event = DOUBLE_CLICK;
				DConUp[idx] = false;
				DCwaiting[idx] = false;
				singleOK[idx] = false;
			}
		}
	}
// Test for normal click event: DCGAP expired
	if (buttonVal[idx] == HIGH && (millis() - upTime[idx]) >= DCGAP && DCwaiting[idx] == true && DConUp[idx] == false
			&& singleOK[idx] == true)
	{
		event = CLICK;
		DCwaiting[idx] = false;
	}
// Test for hold
	if ((buttonVal[idx] == LOW) && (millis() - downTime[idx]) >= HOLDTIME)
	{
// Trigger "normal" hold
		if (not holdEventPast[idx])
		{
			event = HOLD;
			waitForUp[idx] = true;
			ignoreUp[idx] = true;
			DConUp[idx] = false;
			DCwaiting[idx] = false;
			//downTime = millis();
			holdEventPast[idx] = true;
		}
// Trigger "long" hold
		if ((millis() - downTime[idx]) >= LONGHOLDTIME)
		{
			if (not longHoldEventPast[idx])
			{
				event = LONG_HOLD;
				longHoldEventPast[idx] = true;
			}
		}
	}
	buttonLast[idx] = buttonVal[idx];
	return event;
}

/* Return CLICK if button is pressed */
button_event_t checkButtonStatus(uint8_t pin)
{
	button_event_t event = NO_EVENT;
	uint8_t idx = 0;

	// switch idx depending on pin
	if (pin == LED_TOUCH_PANEL_PIN)
	{
		idx = 0;
	}
	else // LED_BUTTON_MODE_PIN
	{
		idx = 1;
	}

	// Read the state of the button
	buttonVal[idx] = digitalRead(pin);

	/* Button is low, notify*/
	if(buttonVal[idx] == LOW)
	{
		event = CLICK;
	}

	return event;
}

void initButtonState(uint8_t pin)
{
	uint8_t idx = 0;

	// switch idx depending on pin
	if (pin == LED_TOUCH_PANEL_PIN)
	{
		idx = 0;
	}
	else // LED_BUTTON_MODE_PIN
	{
		idx = 1;
	}

	/* Reset all button variables */
	buttonVal[idx] = HIGH;
	buttonLast[idx] = HIGH;
	DCwaiting[idx] = false;
	DConUp[idx] = false;
	singleOK[idx] = true;
	downTime[idx] = -1;
	upTime[idx] = -1;
	ignoreUp[idx] = false;
	waitForUp[idx] = false;
	holdEventPast[idx] = false;
	longHoldEventPast[idx] = false;
}
#endif
