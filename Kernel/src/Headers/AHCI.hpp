#pragma once
#ifndef H_AHCI
#define H_AHCI
#include <Globals.h>
#include <PCI.h>
#include <PageTableManager.h>

namespace AHCI 
{
	using namespace PCI;

	namespace PortType
	{
		enum : byte
		{
			None = 0x00,
			SATA = 0x01,
			SEMB = 0x02,
			PM = 0x03,
			SATAPI = 0x04,
		};
	}

	struct HBAPort
	{
		uint32 CommandListBase;
		uint32 CommandListBaseUpper;

		uint32 fisBaseAddress;
		uint32 fisBaseAddressUpper;

		uint32 InterruptStatus;
		uint32 InterruptEnabled;

		uint32 cmdStatus;
		uint32 rsv0;

		uint32 TaskFileData;
		uint32 Signature;

		uint32 sataStatus;
		uint32 sataControl;
		uint32 sataError;
		uint32 sataActive;
		uint32 CommandIssue;
		uint32 sataNotification;

		uint32 fisSwitchControl;
		uint32 rsv1[11];

		uint32 Vendor[4];

	};

	struct HBAMemory
	{
		uint32 HostCapability;
		uint32 GlobalHostControl;
		uint32 InterruptStatus;
		uint32 PortsImplemented;

		uint32 Version;

		uint32 cccControl;
		uint32 cccPorts;
		uint32 EnclosureManagementLocation;
		uint32 EnclosureManagementControl;

		uint32 HostCapabilityExt;
		uint32 BIOSHandoffCtrlSts;

		uint8 rsv0[0x74];
		uint8 Vendor[0x60];

		HBAPort Ports[1];

	};

	forceinline byte GetPortType(fast HBAPort* port)
	{
		constexpr byte hbaDevicePresentAndConnected = 3;
		constexpr byte hbaIpmActive = 1;

		constexpr uint32 sataSigATAPI = 0xEB140101;
		constexpr uint32 sataSigATA = 0x101;
		constexpr uint32 sataSigSEMB = 0xC33C0101;
		constexpr uint32 sataSigPM = 0x9669101;

		fast u32 status = port->sataStatus;
		fast byte powman = (status >> 8) & 0b111;
		fast byte devdet = status & 0b111;

		if (devdet != hbaDevicePresentAndConnected) return PortType::None;
		if (powman != hbaIpmActive) return PortType::None;

		switch (port->Signature)
		{
			case sataSigATAPI:
				return PortType::SATAPI;

			case sataSigATA:
				return PortType::SATA;

			case sataSigSEMB:
				return PortType::SEMB;

			case sataSigPM:
				return PortType::PM;
		}

		return PortType::None;
	}

	struct AHCIDriver
	{
		inline AHCIDriver(PCIDeviceHeader* device)
		{
			this->mPCIBaseAddress = device;
			this->mABAR = (HBAMemory*)nint(((PCIHeader0*)device)->BAR5);
			PageTableManager::MapMemory(mABAR, mABAR);
			ProbePorts();
		}

		inline ~AHCIDriver()
		{

		}

		inline void ProbePorts()
		{
			fast u32 portsImpl = mABAR->PortsImplemented;

			for (fast u32 x = 0; x < 32; x++)
			{
				if (portsImpl & (1 << x))
				{
					byte portType = GetPortType(mABAR->Ports + x);
					switch (portType)
					{
						case PortType::SATA: //debug("\tSATA Drive Found!");
							break;

						case PortType::SATAPI: //debug("\SATAPI Drive Found!");
							break;

						case PortType::SEMB: //debug("\SEMB Drive Found!");
							break;

						case PortType::PM: //debug("\tPM Drive Found!");
							break;

						default: //debug("\tUnknown Drive Found");
							break;
					}
				}
			}
		}

	private:

		PCIDeviceHeader* mPCIBaseAddress;
		HBAMemory* mABAR;

	};
}

#endif