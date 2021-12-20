#pragma once

#include "CoreMinimal.h"
#include "FairyCommons.h"

namespace FairyGUI {

	enum class EPackageSegmentIndex
	{
		DependencyAndBranch = 0, // Package dependency and branch info segment
		CompontentList = 1, // All component data in package
		PixelHitTest = 3, // Pixel hit test data in package
		Atlas = 2, // Atlas info for package
		StringTable = 4, // String table data in package
	};

	enum class EComponentBlockIndex
	{
		Base = 0
	};

	class FAIRYGUI_API FByteBuffer
	{
	public:
		FByteBuffer(const uint8* InBuffer, int32 InOffset, int32 InLen, bool bInTransferBuffer);
		~FByteBuffer();

		const uint8* GetBuffer() const { return Buffer; }

		int32 GetBytesAvailable() const;
		int32 GetLength() const { return Length; }

		int32 GetPos() const { return Position; }
		void SetPos(int32 InPos) { Position = InPos; }
		void Skip(int32 InBytes) { Position += InBytes; }

		/**
		* Base api.
		*/
		inline int8 ReadByte();
		inline uint8 ReadUbyte();
		inline bool ReadBool();
		inline int16 ReadShort();
		inline uint16 ReadUshort();
		inline int32 ReadInt();
		inline uint32 ReadUint();
		inline float ReadFloat();
		inline uint16 ReadStringIdx();

		/**
		* Position add 2 bytes and string length bytes; The first two bytes is a ushort value, means string length.
		*
		* @return A new FString object.
		*/
		FString ReadString();
		
		[[deprecated("Use ReadStringFromCache() instead.")]]
		const FString& ReadS();
		
		/**
		* Position add 2 bytes for get string table index.
		*
		* @return A FString reference that handled in StringTable.
		*/
		const FString& ReadStringFromCache();

		[[deprecated("Use ReadFNameFromCache() instead.")]]
		FName ReadFName();

		/**
		* Position add 2 bytes for get string table index.
		*
		* @return A FName object equal to FString object that handled in StringTable.
		*/
		FName ReadFNameFromCache();

		void ReadSArray(TArray<FString>& OutArray, int32 InCount);
		void ReadFNameArray(TArray<FName>& OutArray, int32 InCount);
		bool ReadS(FString& OutString);
		const FString* ReadSP();
		void WriteS(const FString& InString);

		FColor ReadColor();
		TSharedPtr<FByteBuffer> ReadBuffer(bool bCloneBuffer);
		bool Seek(int32 IndexTablePos, int32 BlockIndex);

		bool bLittleEndian;
		int32 Version;
		TSharedPtr<TArray<FString>> StringTable;

	private:
		/**
		* Position add InLen bytes.
		*
		* @param InLen bytes to read.
		* @return A new FString object.
		*/
		FString ReadString(int32 InLen);

	private:
		const uint8* Buffer;
		int32 Offset;
		int32 Length;
		int32 Position;
		bool bTransferBuffer;
	};

}

