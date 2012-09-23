#include "PoKeysLib.h"
#include "PoKeysLibCore.h"

int PK_EncoderConfigurationGet(sPoKeysDevice* device)
{
	if (device->info.iBasicEncoderCount)
	{
		// Get basic encoder options
		CreateRequest(device->request, 0xC4, 0, 0, 0, 0);
		if (SendRequest(device) == PK_OK)
		{
			for (int i = 0; i < device->info.iBasicEncoderCount; i++)
			{
				device->Encoders[i].encoderOptions = device->response[8 + i];
			}
		} else return PK_ERR_TRANSFER;

		// Get channel mappings
		CreateRequest(device->request, 0xC5, 0, 0, 0, 0);
		if (SendRequest(device) == PK_OK)
		{
			for (int i = 0; i < device->info.iBasicEncoderCount; i++)
			{
				device->Encoders[i].channelApin = device->response[8 + i];
				device->Encoders[i].channelBpin = device->response[33 + i];
			}
		} else return PK_ERR_TRANSFER;

		if (device->info.iKeyMapping)
		{
			// Direction A key mapping
			CreateRequest(device->request, 0xC6, 0, 0, 0, 0);
			if (SendRequest(device) == PK_OK)
			{
				for (int i = 0; i < device->info.iBasicEncoderCount; i++)
				{
					device->Encoders[i].dirAkeyCode = device->response[8 + i];
					device->Encoders[i].dirAkeyModifier = device->response[33 + i];
				}
			} else return PK_ERR_TRANSFER;

			// Direction B key mapping
			CreateRequest(device->request, 0xC7, 0, 0, 0, 0);
			if (SendRequest(device) == PK_OK)
			{
				for (int i = 0; i < device->info.iBasicEncoderCount; i++)
				{
					device->Encoders[i].dirBkeyCode = device->response[8 + i];
					device->Encoders[i].dirBkeyModifier = device->response[33 + i];
				}
			} else return PK_ERR_TRANSFER;
		}

		if (device->info.iFastEncoders)
		{
			// Get fast encoders configuration
			CreateRequest(device->request, 0xCE, 2, 0, 0, 0);
			if (SendRequest(device) == PK_OK)
			{
				device->FastEncodersConfiguration = device->response[2];
				device->FastEncodersOptions = device->response[3];
			} else return PK_ERR_TRANSFER;
		}
		if (device->info.iUltraFastEncoders)
		{
			// Get ultra fast encoders configuration
			CreateRequest(device->request, 0x1C, 0xFF, 0, 0, 0);
			if (SendRequest(device) == PK_OK)
			{
				device->UltraFastEncoderConfiguration = device->response[2];
				device->UltraFastEncoderOptions = device->response[3];
				device->UltraFastEncoderFilter = *((unsigned int*)&(device->response[8]));
			} else return PK_ERR_TRANSFER;
		}

	}
	return PK_OK;
}

int PK_EncoderConfigurationSet(sPoKeysDevice* device)
{
	if (device->info.iBasicEncoderCount)
	{
		// Set basic encoder options
		CreateRequest(device->request, 0xC4, 1, 0, 0, 0);
		for (int i = 0; i < device->info.iBasicEncoderCount; i++)
		{
			device->request[8 + i] = device->Encoders[i].encoderOptions;
		}
		if (SendRequest(device) != PK_OK) return PK_ERR_TRANSFER;

		// Set channel mappings
		CreateRequest(device->request, 0xC5, 0, 0, 0, 0);
		for (int i = 0; i < device->info.iBasicEncoderCount; i++)
		{
			device->request[8 + i] = device->Encoders[i].channelApin;
			device->request[33 + i] = device->Encoders[i].channelBpin;
		}
		if (SendRequest(device) != PK_OK) return PK_ERR_TRANSFER;

		if (device->info.iKeyMapping)
		{
			// Direction A key mapping
			CreateRequest(device->request, 0xC6, 0, 0, 0, 0);
			for (int i = 0; i < device->info.iBasicEncoderCount; i++)
			{
				device->request[8 + i] = device->Encoders[i].dirAkeyCode;
				device->request[33 + i] = device->Encoders[i].dirAkeyModifier;
			}
			if (SendRequest(device) != PK_OK) return PK_ERR_TRANSFER;

			// Direction B key mapping
			CreateRequest(device->request, 0xC7, 0, 0, 0, 0);
			for (int i = 0; i < device->info.iBasicEncoderCount; i++)
			{
				device->request[8 + i] = device->Encoders[i].dirBkeyCode;
				device->request[33 + i] = device->Encoders[i].dirBkeyModifier;
			}
			if (SendRequest(device) != PK_OK) return PK_ERR_TRANSFER;
		}

		if (device->info.iFastEncoders)
		{
			// Set fast encoders configuration
			CreateRequest(device->request, 0xCE, device->FastEncodersConfiguration, device->FastEncodersOptions, 0, 0);
			if (SendRequest(device) != PK_OK) return PK_ERR_TRANSFER;
		}
		if (device->info.iUltraFastEncoders)
		{
			// Set ultra fast encoders configuration
			CreateRequest(device->request, 0x1C, device->UltraFastEncoderConfiguration, device->UltraFastEncoderOptions, 0, 0);
			*((unsigned int*)&(device->request[8])) = device->UltraFastEncoderFilter;
			if (SendRequest(device) != PK_OK) return PK_ERR_TRANSFER;
		}

	}
	return PK_OK;
}

int PK_EncoderValuesGet(sPoKeysDevice* device)
{
	if (device->info.iBasicEncoderCount >= 13)
	{
		// Read the first 13 encoders
		CreateRequest(device->request, 0xCD, 0, 0, 0, 0);
		if (SendRequest(device) == PK_OK)
		{
			for (int i = 0; i < 13; i++)
			{
				device->Encoders[i].encoderValue = *((unsigned int*)&device->response[8 + i * 4]);
			}
		} else return PK_ERR_TRANSFER;
	}
	
	if (device->info.iBasicEncoderCount >= 25 && device->info.iFastEncoders == 0)
	{
		// Read the next 12 encoders
		CreateRequest(device->request, 0xCD, 1, 0, 0, 0);
		if (SendRequest(device) == PK_OK)
		{
			for (int i = 0; i < 12; i++)
			{
				device->Encoders[13 + i].encoderValue = *((unsigned int*)&device->response[8 + i * 4]);
			}
		} else return PK_ERR_TRANSFER;
	} else if (device->info.iBasicEncoderCount >= 25 && device->info.iUltraFastEncoders != 0)
	{
		// Read the next 12 encoders and ultra fast encoder
		CreateRequest(device->request, 0xCD, 1, 0, 0, 0);
		if (SendRequest(device) == PK_OK)
		{
			for (int i = 0; i < 13; i++)
			{
				device->Encoders[13 + i].encoderValue = *((unsigned int*)&device->response[8 + i * 4]);
			}
		} else return PK_ERR_TRANSFER;
	}
	return PK_OK;
}

int PK_EncoderValuesSet(sPoKeysDevice* device)
{
	if (device->info.iBasicEncoderCount >= 13)
	{
		// Read the first 13 encoders
		CreateRequest(device->request, 0xCD, 10, 0, 0, 0);
		for (int i = 0; i < 13; i++)
		{
			*((unsigned int*)&device->request[8 + i * 4]) = device->Encoders[i].encoderValue;
		}
		if (SendRequest(device) != PK_OK) return PK_ERR_TRANSFER;
	}
	
	if (device->info.iBasicEncoderCount >= 25)
	{
		// Read the next 12 encoders
		CreateRequest(device->request, 0xCD, 11, 0, 0, 0);
		for (int i = 0; i < 13; i++)
		{
			*((unsigned int*)&device->request[8 + i * 4]) = device->Encoders[13 + i].encoderValue;
		}
		if (SendRequest(device) != PK_OK) return PK_ERR_TRANSFER;
	}
	return PK_OK;
}