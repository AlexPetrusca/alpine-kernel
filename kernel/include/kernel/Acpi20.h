/** @file
   ACPI 2.0 definitions from the ACPI Specification, revision 2.0

   Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
   This program and the accompanying materials
   are licensed and made available under the terms and conditions of the BSD License
   which accompanies this distribution.  The full text of the license may be found at
   http://opensource.org/licenses/bsd-license.php

   THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
   WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 **/

#ifndef _ACPI_2_0_H_
#define _ACPI_2_0_H_

#include "Acpi10.h"

//
// Define for Desriptor
//
#define ACPI_LARGE_GENERIC_REGISTER_DESCRIPTOR_NAME          0x02

#define ACPI_GENERIC_REGISTER_DESCRIPTOR          0x82

//
// Ensure proper structure formats
//
#pragma pack(1)

///
/// Generic Register Descriptor
///
typedef struct {
  ACPI_LARGE_RESOURCE_HEADER Header;
  uint8_t AddressSpaceId;
  uint8_t RegisterBitWidth;
  uint8_t RegisterBitOffset;
  uint8_t AddressSize;
  uint64_t RegisterAddress;
} EFI_ACPI_GENERIC_REGISTER_DESCRIPTOR;

#pragma pack()

//
// Ensure proper structure formats
//
#pragma pack(1)

///
/// ACPI 2.0 Generic Address Space definition
///
typedef struct {
  uint8_t AddressSpaceId;
  uint8_t RegisterBitWidth;
  uint8_t RegisterBitOffset;
  uint8_t Reserved;
  uint64_t Address;
} EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE;

//
// Generic Address Space Address IDs
//
#define EFI_ACPI_2_0_SYSTEM_MEMORY              0
#define EFI_ACPI_2_0_SYSTEM_IO                  1
#define EFI_ACPI_2_0_PCI_CONFIGURATION_SPACE    2
#define EFI_ACPI_2_0_EMBEDDED_CONTROLLER        3
#define EFI_ACPI_2_0_SMBUS                      4
#define EFI_ACPI_2_0_FUNCTIONAL_FIXED_HARDWARE  0x7F

//
// ACPI 2.0 table structures
//

///
/// Root System Description Pointer Structure
///
typedef struct {
  uint64_t Signature;
  uint8_t Checksum;
  uint8_t OemId[6];
  uint8_t Revision;
  uint32_t RsdtAddress;
  uint32_t Length;
  uint64_t XsdtAddress;
  uint8_t ExtendedChecksum;
  uint8_t Reserved[3];
} Acpi2Rsdp;

///
/// RSD_PTR Revision (as defined in ACPI 2.0 spec.)
///
#define EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER_REVISION 0x02

///
/// Common table header, this prefaces all ACPI tables, including FACS, but
/// excluding the RSD PTR structure
///
typedef struct {
  uint32_t Signature;
  uint32_t Length;
} EFI_ACPI_2_0_COMMON_HEADER;

//
// Root System Description Table
// No definition needed as it is a common description table header, the same with
// EFI_ACPI_DESCRIPTION_HEADER, followed by a variable number of UINT32 table pointers.
//

///
/// RSDT Revision (as defined in ACPI 2.0 spec.)
///
#define EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_TABLE_REVISION 0x01

//
// Extended System Description Table
// No definition needed as it is a common description table header, the same with
// EFI_ACPI_DESCRIPTION_HEADER, followed by a variable number of UINT64 table pointers.
//

///
/// XSDT Revision (as defined in ACPI 2.0 spec.)
///
#define EFI_ACPI_2_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_REVISION 0x01

///
/// Fixed ACPI Description Table Structure (FADT)
///
typedef struct {
  AcpiDescriptionHeader Header;
  uint32_t FirmwareCtrl;
  uint32_t Dsdt;
  uint8_t Reserved0;
  uint8_t PreferredPmProfile;
  uint16_t SciInt;
  uint32_t SmiCmd;
  uint8_t AcpiEnable;
  uint8_t AcpiDisable;
  uint8_t S4BiosReq;
  uint8_t PstateCnt;
  uint32_t Pm1aEvtBlk;
  uint32_t Pm1bEvtBlk;
  uint32_t Pm1aCntBlk;
  uint32_t Pm1bCntBlk;
  uint32_t Pm2CntBlk;
  uint32_t PmTmrBlk;
  uint32_t Gpe0Blk;
  uint32_t Gpe1Blk;
  uint8_t Pm1EvtLen;
  uint8_t Pm1CntLen;
  uint8_t Pm2CntLen;
  uint8_t PmTmrLen;
  uint8_t Gpe0BlkLen;
  uint8_t Gpe1BlkLen;
  uint8_t Gpe1Base;
  uint8_t CstCnt;
  uint16_t PLvl2Lat;
  uint16_t PLvl3Lat;
  uint16_t FlushSize;
  uint16_t FlushStride;
  uint8_t DutyOffset;
  uint8_t DutyWidth;
  uint8_t DayAlrm;
  uint8_t MonAlrm;
  uint8_t Century;
  uint16_t IaPcBootArch;
  uint8_t Reserved1;
  uint32_t Flags;
  EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE ResetReg;
  uint8_t ResetValue;
  uint8_t Reserved2[3];
  uint64_t XFirmwareCtrl;
  uint64_t XDsdt;
  EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE XPm1aEvtBlk;
  EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE XPm1bEvtBlk;
  EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE XPm1aCntBlk;
  EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE XPm1bCntBlk;
  EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE XPm2CntBlk;
  EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE XPmTmrBlk;
  EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE XGpe0Blk;
  EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE XGpe1Blk;
} EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE;

///
/// FADT Version (as defined in ACPI 2.0 spec.)
///
#define EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION  0x03

//
// Fixed ACPI Description Table Preferred Power Management Profile
//
#define EFI_ACPI_2_0_PM_PROFILE_UNSPECIFIED         0
#define EFI_ACPI_2_0_PM_PROFILE_DESKTOP             1
#define EFI_ACPI_2_0_PM_PROFILE_MOBILE              2
#define EFI_ACPI_2_0_PM_PROFILE_WORKSTATION         3
#define EFI_ACPI_2_0_PM_PROFILE_ENTERPRISE_SERVER   4
#define EFI_ACPI_2_0_PM_PROFILE_SOHO_SERVER         5
#define EFI_ACPI_2_0_PM_PROFILE_APPLIANCE_PC        6

//
// Fixed ACPI Description Table Boot Architecture Flags
// All other bits are reserved and must be set to 0.
//
#define EFI_ACPI_2_0_LEGACY_DEVICES          BIT0
#define EFI_ACPI_2_0_8042                    BIT1

//
// Fixed ACPI Description Table Fixed Feature Flags
// All other bits are reserved and must be set to 0.
//
#define EFI_ACPI_2_0_WBINVD                  BIT0
#define EFI_ACPI_2_0_WBINVD_FLUSH            BIT1
#define EFI_ACPI_2_0_PROC_C1                 BIT2
#define EFI_ACPI_2_0_P_LVL2_UP               BIT3
#define EFI_ACPI_2_0_PWR_BUTTON              BIT4
#define EFI_ACPI_2_0_SLP_BUTTON              BIT5
#define EFI_ACPI_2_0_FIX_RTC                 BIT6
#define EFI_ACPI_2_0_RTC_S4                  BIT7
#define EFI_ACPI_2_0_TMR_VAL_EXT             BIT8
#define EFI_ACPI_2_0_DCK_CAP                 BIT9
#define EFI_ACPI_2_0_RESET_REG_SUP           BIT10
#define EFI_ACPI_2_0_SEALED_CASE             BIT11
#define EFI_ACPI_2_0_HEADLESS                BIT12
#define EFI_ACPI_2_0_CPU_SW_SLP              BIT13

///
/// Firmware ACPI Control Structure
///
typedef struct {
  uint32_t Signature;
  uint32_t Length;
  uint32_t HardwareSignature;
  uint32_t FirmwareWakingVector;
  uint32_t GlobalLock;
  uint32_t Flags;
  uint64_t XFirmwareWakingVector;
  uint8_t Version;
  uint8_t Reserved[31];
} EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE;

///
/// FACS Version (as defined in ACPI 2.0 spec.)
///
#define EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_VERSION  0x01

///
/// Firmware Control Structure Feature Flags
/// All other bits are reserved and must be set to 0.
///
#define EFI_ACPI_2_0_S4BIOS_F        BIT0

///
/// Multiple APIC Description Table header definition.  The rest of the table
/// must be defined in a platform specific manner.
///
typedef struct {
  AcpiDescriptionHeader Header;
  uint32_t LocalApicAddress;
  uint32_t Flags;
} Acpi2MultipleApicTableHeader;

///
/// MADT Revision (as defined in ACPI 2.0 spec.)
///
#define EFI_ACPI_2_0_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION 0x01

///
/// Multiple APIC Flags
/// All other bits are reserved and must be set to 0.
///
#define EFI_ACPI_2_0_PCAT_COMPAT          BIT0

//
// Multiple APIC Description Table APIC structure types
// All other values between 0x09 an 0xFF are reserved and
// will be ignored by OSPM.
//
#define EFI_ACPI_2_0_PROCESSOR_LOCAL_APIC           0x00
#define EFI_ACPI_2_0_IO_APIC                        0x01
#define EFI_ACPI_2_0_INTERRUPT_SOURCE_OVERRIDE      0x02
#define EFI_ACPI_2_0_NON_MASKABLE_INTERRUPT_SOURCE  0x03
#define EFI_ACPI_2_0_LOCAL_APIC_NMI                 0x04
#define EFI_ACPI_2_0_LOCAL_APIC_ADDRESS_OVERRIDE    0x05
#define EFI_ACPI_2_0_IO_SAPIC                       0x06
#define EFI_ACPI_2_0_PROCESSOR_LOCAL_SAPIC          0x07
#define EFI_ACPI_2_0_PLATFORM_INTERRUPT_SOURCES     0x08

//
// APIC Structure Definitions
//

///
/// Processor Local APIC Structure Definition
///
typedef struct {
  uint8_t Type;
  uint8_t Length;
  uint8_t AcpiProcessorId;
  uint8_t ApicId;
  uint32_t Flags;
} EFI_ACPI_2_0_PROCESSOR_LOCAL_APIC_STRUCTURE;

///
/// Local APIC Flags.  All other bits are reserved and must be 0.
///
#define EFI_ACPI_2_0_LOCAL_APIC_ENABLED         BIT0

///
/// IO APIC Structure
///
typedef struct {
  uint8_t Type;
  uint8_t Length;
  uint8_t IoApicId;
  uint8_t Reserved;
  uint32_t IoApicAddress;
  uint32_t GlobalSystemInterruptBase;
} EFI_ACPI_2_0_IO_APIC_STRUCTURE;

///
/// Interrupt Source Override Structure
///
typedef struct {
  uint8_t Type;
  uint8_t Length;
  uint8_t Bus;
  uint8_t Source;
  uint32_t GlobalSystemInterrupt;
  uint16_t Flags;
} EFI_ACPI_2_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE;

///
/// Non-Maskable Interrupt Source Structure
///
typedef struct {
  uint8_t Type;
  uint8_t Length;
  uint16_t Flags;
  uint32_t GlobalSystemInterrupt;
} EFI_ACPI_2_0_NON_MASKABLE_INTERRUPT_SOURCE_STRUCTURE;

///
/// Local APIC NMI Structure
///
typedef struct {
  uint8_t Type;
  uint8_t Length;
  uint8_t AcpiProcessorId;
  uint16_t Flags;
  uint8_t LocalApicLint;
} EFI_ACPI_2_0_LOCAL_APIC_NMI_STRUCTURE;

///
/// Local APIC Address Override Structure
///
typedef struct {
  uint8_t Type;
  uint8_t Length;
  uint16_t Reserved;
  uint64_t LocalApicAddress;
} EFI_ACPI_2_0_LOCAL_APIC_ADDRESS_OVERRIDE_STRUCTURE;

///
/// IO SAPIC Structure
///
typedef struct {
  uint8_t Type;
  uint8_t Length;
  uint8_t IoApicId;
  uint8_t Reserved;
  uint32_t GlobalSystemInterruptBase;
  uint64_t IoSapicAddress;
} EFI_ACPI_2_0_IO_SAPIC_STRUCTURE;

///
/// Local SAPIC Structure
///
typedef struct {
  uint8_t Type;
  uint8_t Length;
  uint8_t AcpiProcessorId;
  uint8_t LocalSapicId;
  uint8_t LocalSapicEid;
  uint8_t Reserved[3];
  uint32_t Flags;
} EFI_ACPI_2_0_PROCESSOR_LOCAL_SAPIC_STRUCTURE;

///
/// Platform Interrupt Sources Structure
///
typedef struct {
  uint8_t Type;
  uint8_t Length;
  uint16_t Flags;
  uint8_t InterruptType;
  uint8_t ProcessorId;
  uint8_t ProcessorEid;
  uint8_t IoSapicVector;
  uint32_t GlobalSystemInterrupt;
  uint32_t Reserved;
} EFI_ACPI_2_0_PLATFORM_INTERRUPT_SOURCES_STRUCTURE;

///
/// Smart Battery Description Table (SBST)
///
typedef struct {
  AcpiDescriptionHeader Header;
  uint32_t WarningEnergyLevel;
  uint32_t LowEnergyLevel;
  uint32_t CriticalEnergyLevel;
} EFI_ACPI_2_0_SMART_BATTERY_DESCRIPTION_TABLE;

///
/// SBST Version (as defined in ACPI 2.0 spec.)
///
#define EFI_ACPI_2_0_SMART_BATTERY_DESCRIPTION_TABLE_REVISION 0x01

///
/// Embedded Controller Boot Resources Table (ECDT)
/// The table is followed by a null terminated ASCII string that contains
/// a fully qualified reference to the name space object.
///
typedef struct {
  AcpiDescriptionHeader Header;
  EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE EcControl;
  EFI_ACPI_2_0_GENERIC_ADDRESS_STRUCTURE EcData;
  uint32_t Uid;
  uint8_t GpeBit;
} EFI_ACPI_2_0_EMBEDDED_CONTROLLER_BOOT_RESOURCES_TABLE;

///
/// ECDT Version (as defined in ACPI 2.0 spec.)
///
#define EFI_ACPI_2_0_EMBEDDED_CONTROLLER_BOOT_RESOURCES_TABLE_REVISION  0x01

//
// Known table signatures
//

///
/// "RSD PTR " Root System Description Pointer
///
#define EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER_SIGNATURE  SIGNATURE_64('R', 'S', 'D', ' ', 'P', 'T', 'R', ' ')

///
/// "SPIC" Multiple SAPIC Description Table
///
/// BUGBUG: Don't know where this came from except SR870BN4 uses it.
/// #define EFI_ACPI_2_0_MULTIPLE_SAPIC_DESCRIPTION_TABLE_SIGNATURE 0x43495053
///
#define EFI_ACPI_2_0_MULTIPLE_SAPIC_DESCRIPTION_TABLE_SIGNATURE  SIGNATURE_32('A', 'P', 'I', 'C')

///
/// "BOOT" MS Simple Boot Spec
///
#define EFI_ACPI_2_0_SIMPLE_BOOT_FLAG_TABLE_SIGNATURE  SIGNATURE_32('B', 'O', 'O', 'T')

///
/// "DBGP" MS Bebug Port Spec
///
#define EFI_ACPI_2_0_DEBUG_PORT_TABLE_SIGNATURE  SIGNATURE_32('D', 'B', 'G', 'P')

///
/// "DSDT" Differentiated System Description Table
///
#define EFI_ACPI_2_0_DIFFERENTIATED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE  SIGNATURE_32('D', 'S', 'D', 'T')

///
/// "ECDT" Embedded Controller Boot Resources Table
///
#define EFI_ACPI_2_0_EMBEDDED_CONTROLLER_BOOT_RESOURCES_TABLE_SIGNATURE  SIGNATURE_32('E', 'C', 'D', 'T')

///
/// "ETDT" Event Timer Description Table
///
#define EFI_ACPI_2_0_EVENT_TIMER_DESCRIPTION_TABLE_SIGNATURE  SIGNATURE_32('E', 'T', 'D', 'T')

///
/// "FACS" Firmware ACPI Control Structure
///
#define EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE  SIGNATURE_32('F', 'A', 'C', 'S')

///
/// "FACP" Fixed ACPI Description Table
///
#define EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE  SIGNATURE_32('F', 'A', 'C', 'P')

///
/// "APIC" Multiple APIC Description Table
///
#define EFI_ACPI_2_0_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE  SIGNATURE_32('A', 'P', 'I', 'C')

///
/// "PSDT" Persistent System Description Table
///
#define EFI_ACPI_2_0_PERSISTENT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE  SIGNATURE_32('P', 'S', 'D', 'T')

///
/// "RSDT" Root System Description Table
///
#define EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE  SIGNATURE_32('R', 'S', 'D', 'T')

///
/// "SBST" Smart Battery Specification Table
///
#define EFI_ACPI_2_0_SMART_BATTERY_SPECIFICATION_TABLE_SIGNATURE  SIGNATURE_32('S', 'B', 'S', 'T')

///
/// "SLIT" System Locality Information Table
///
#define EFI_ACPI_2_0_SYSTEM_LOCALITY_INFORMATION_TABLE_SIGNATURE  SIGNATURE_32('S', 'L', 'I', 'T')

///
/// "SPCR" Serial Port Concole Redirection Table
///
#define EFI_ACPI_2_0_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_SIGNATURE  SIGNATURE_32('S', 'P', 'C', 'R')

///
/// "SRAT" Static Resource Affinity Table
///
#define EFI_ACPI_2_0_STATIC_RESOURCE_AFFINITY_TABLE_SIGNATURE  SIGNATURE_32('S', 'R', 'A', 'T')

///
/// "SSDT" Secondary System Description Table
///
#define EFI_ACPI_2_0_SECONDARY_SYSTEM_DESCRIPTION_TABLE_SIGNATURE  SIGNATURE_32('S', 'S', 'D', 'T')

///
/// "SPMI" Server Platform Management Interface Table
///
#define EFI_ACPI_2_0_SERVER_PLATFORM_MANAGEMENT_INTERFACE_SIGNATURE  SIGNATURE_32('S', 'P', 'M', 'I')

///
/// "XSDT" Extended System Description Table
///
#define EFI_ACPI_2_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE  SIGNATURE_32('X', 'S', 'D', 'T')

///
/// "MCFG" PCI Express Memory Mapped Configuration Space Base Address Description Table
///
#define EFI_ACPI_2_0_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_SIGNATURE  SIGNATURE_32('M', 'C', 'F', 'G')

#pragma pack()

#endif