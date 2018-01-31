/*
  ==============================================================================

    SynthAudioProcessEditor.h
    Created: 23 Jan 2018 7:22:31pm
    Author:  vdorn

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "SynthProcessor.h"

/*
==============================================================================

This file is part of the JUCE library.
Copyright (c) 2015 - ROLI Ltd.

Permission is granted to use this software under the terms of either:
a) the GPL v2 (or any later version)
b) the Affero GPL v3

Details of these licenses can be found at: www.gnu.org/licenses

JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

------------------------------------------------------------------------------

To release a closed-source product which uses JUCE, commercial licenses are
available: visit www.juce.com for more information.

==============================================================================
*/


//Using the generic editor so that we can use it for the plugins as well maybe? - Victoria
class GenericEditor : public AudioProcessorEditor,
	public Slider::Listener,
	private ComboBox::Listener,
	private Timer
{
public:
	enum
	{
		kParamControlHeight = 40,
		kParamLabelWidth = 80,
		kParamSliderWidth = 300
	};

	GenericEditor(AudioProcessor& parent)
		: AudioProcessorEditor(parent),
		noParameterLabel("noparam", "No parameters available")
	{
		const OwnedArray<AudioProcessorParameter>& params = parent.getParameters();

		// Creates a rotary slider for each parameter of the processor
		for (int i = 0; i < params.size(); ++i)
		{
			if (const AudioParameterFloat* param = dynamic_cast<AudioParameterFloat*> (params[i]))
			{
				Slider* aSlider;

				//testing out comparing param names to do custom things with the sliders
				/*if (param->name.compare("Square Pitch") == 0) {
					printf("Param name %d: %s", i, param->name);
					paramSliders.add(aSlider = new Slider(param->name));
					aSlider->setRange(param->range.start, param->range.end);
					aSlider->setSliderStyle(Slider::LinearVertical);
					aSlider->setValue(*param);

					aSlider->addListener(this);
					addAndMakeVisible(aSlider);

					Label* aLabel;
					paramLabels.add(aLabel = new Label(param->name, param->name));
					addAndMakeVisible(aLabel);
				}
				else {*/
					paramSliders.add(aSlider = new Slider(param->name));
					aSlider->setRange(param->range.start, param->range.end);
					aSlider->setSliderStyle(Slider::Rotary);
					aSlider->setValue(*param);

					aSlider->addListener(this);
					addAndMakeVisible(aSlider);

					Label* aLabel;
					paramLabels.add(aLabel = new Label(param->name, param->name));
					addAndMakeVisible(aLabel);
				//}
			}
			else if (const AudioParameterChoice* choiceParam = dynamic_cast<AudioParameterChoice*> (params[i]))
			{
				ComboBox * cbOsc;
				paramCombo.add(cbOsc = new ComboBox(choiceParam->name));
				//const String & theChoices;
				int i = 1;
				for (const juce::String * theChoice = choiceParam->choices.begin(); theChoice < choiceParam->choices.end(); theChoice++) {
					cbOsc->addItem(*theChoice, i);
					i++;
				}

				//cbOsc->setJustificationType(Justification::centred);
				addAndMakeVisible(cbOsc);
				cbOsc->addListener(this);
			}

		}

		//sliderTree = new AudioProcessorValueTreeState::SliderAttachment()

		noParameterLabel.setJustificationType(Justification::horizontallyCentred | Justification::verticallyCentred);
		noParameterLabel.setFont(noParameterLabel.getFont().withStyle(Font::italic));

		setSize(kParamSliderWidth + kParamLabelWidth,
			jmax(1, kParamControlHeight * paramSliders.size()));

		if (paramSliders.size() == 0)
			addAndMakeVisible(noParameterLabel);
		else
			startTimer(100);
	}

	~GenericEditor()
	{
	}

	void resized() override
	{
		juce::Rectangle<int> r = getLocalBounds();

		//juce::Rectangle<int> sideBarArea(r.removeFromRight(r.getWidth() / 4));

		//paramCombo[0]->setBounds(r.removeFromTop(20));

		Label * aLabel2 = new Label("Oscillators", "Oscillators");

		addAndMakeVisible(aLabel2);
		aLabel2->setBounds(2, 10, (r.getWidth() / 4.8), 50);
		paramCombo[0]->setBounds(75, 10, (r.getWidth() / 4.8), 50);
		paramCombo[1]->setBounds(275, 10, (r.getWidth() / 4.8), 50);
		paramCombo[2]->setBounds(475, 10, (r.getWidth() / 4.8), 50);
		paramCombo[3]->setBounds(675, 10, (r.getWidth() / 4.8), 50);

		//sine pitch
		paramLabels[0]->setBounds(75, 70, (r.getWidth() / 4.8), 25);
		paramSliders[0]->setBounds(75, 70, (r.getWidth() / 4.8), 50);
		//sine level
		paramLabels[0]->setBounds(100, 70, (r.getWidth() / 4.8), 25);
		paramSliders[0]->setBounds(100, 70, (r.getWidth() / 4.8), 50);


		noParameterLabel.setBounds(r);

		//ugly fix this
		/*for (int i = 0; i < paramSliders.size(); ++i)
		{
			juce::Rectangle<int> paramBounds = r.removeFromTop(kParamControlHeight);
			juce::Rectangle<int> labelBounds = paramBounds.removeFromLeft(kParamLabelWidth);

			paramLabels[i]->setBounds(labelBounds);
			paramSliders[i]->setBounds(paramBounds);
		}*/
	}

	void paint(Graphics& g) override
	{
		//g.setColour(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));
		g.setColour(Colour::fromRGB(115, 115, 115));
		g.fillAll();
	}

	//==============================================================================
	void sliderValueChanged(Slider* slider) override
	{
		if (AudioParameterFloat* param = getParameterForSlider(slider))
			*param = (float)slider->getValue();
	}

	void sliderDragStarted(Slider* slider) override
	{
		if (AudioParameterFloat* param = getParameterForSlider(slider))
			param->beginChangeGesture();
	}

	void sliderDragEnded(Slider* slider) override
	{
		if (AudioParameterFloat* param = getParameterForSlider(slider))
			param->endChangeGesture();
	}

	void juce::ComboBox::Listener::comboBoxChanged(juce::ComboBox *cb) {

	}

private:
	// I believe this updates the GUI if we programically change the parameter values. This happens
	// on a timer, and we don't want to shove this code in the audio thread otherwise that would
	// consume too much CPU.
	void timerCallback() override
	{
		const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();

		for (int i = 0; i < params.size(); ++i)
		{
			if (const AudioParameterFloat* param = dynamic_cast<AudioParameterFloat*> (params[i]))
			{
				if (i < paramSliders.size())
					paramSliders[i]->setValue(*param);
			}
		}
	}

	AudioParameterFloat* getParameterForSlider(Slider* slider)
	{
		const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();
		return dynamic_cast<AudioParameterFloat*> (params[paramSliders.indexOf(slider)]);
	}

	Label noParameterLabel;
	OwnedArray<Slider> paramSliders;
	OwnedArray<Label> paramLabels;
	OwnedArray<ComboBox> paramCombo;
	//ScopedPointer<AudioProcessorValueTreeState::SliderAttachment> sliderTree;

// Adding this below to test passing params via ValueStateTree
//private:
};
