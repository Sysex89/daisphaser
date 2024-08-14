#include "daisysp.h"
#include "daisy_seed.h"

using namespace daisy;
using namespace daisy::seed;
using namespace daisysp;
 
// Create out Daisy Seed Hardware object
DaisySeed hw;
Phaser	phaser; 

 static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size)
{
    for(size_t i = 0; i < size; i += 2)
    {
 

        // Process input signal
        float input_signal = (in[i] + in[i + 1]); // Mono mix of stereo input

        // Apply reverb to the signal
        out[i] = out[i + 1] = phaser.Process(input_signal * .6f);
    }
}

int main(void) {
  // Initialize the Daisy Seed hardware
 
  float sample_rate;
  hw.Configure();
  hw.Init();
  hw.SetAudioBlockSize(4);
  sample_rate = hw.AudioSampleRate();
 
 
 
 GPIO my_led;
 GPIO my_switch;
 my_led.Init(D7, GPIO::Mode::OUTPUT);
 my_switch.Init(D11, GPIO::Mode::INPUT);
  // Start logging for printing over serial
 // hw.StartLog();
 const int num_adc_channels = 2;
 AdcChannelConfig my_adc_config[num_adc_channels];
 my_adc_config[0].InitSingle(A0);
 my_adc_config[1].InitSingle(A1);

  // Initialize the ADC peripheral with that configuration
  hw.adc.Init(my_adc_config, num_adc_channels);
 
  // Start the ADC
  hw.adc.Start();
 
	phaser.Init(sample_rate);
	phaser.SetFreq(500.0f);
	phaser.SetLfoDepth(1.f);

	hw.StartAudio(AudioCallback);

  while(1) {
    // Read the first ADC that's configured. In our case, this is the only input.
    float value1 = hw.adc.GetFloat(0);
	float value2 = hw.adc.GetFloat(1);
	bool sw_val = my_switch.Read();
    phaser.SetLfoDepth(value1);
	phaser.SetFreq(value2 * 1000.0f);
	// In order to know that everything's working let's print that to a serial console:
   // hw.PrintLine("ADC Value1: %f  ADC Value2: %f  SW: %d", value1, value2, sw_val);
	my_led.Write(true);

    // Wait half a second (500 milliseconds)
    System::Delay(500);
  }
}
