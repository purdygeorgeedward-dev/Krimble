/*
 * This file is part of Krita
 *
 * SPDX-FileCopyrightText: 2020 L. E. Segovia <amy@amyspark.me>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#import <CoreFoundation/CoreFoundation.h>
#import <CoreFoundation/CFPlugInCOM.h>
#import <CoreServices/CoreServices.h>

#define PLUGIN_ID "23FE92B0-6256-49C0-B1A6-942FB9B86A82"

Boolean GetMetadataForFile(void *thisInterface,
               CFMutableDictionaryRef attributes,
               CFStringRef contentTypeUTI,
               CFStringRef pathToFile);

typedef struct __MetadataImporterPluginType
{
    MDImporterInterfaceStruct *conduitInterface;
    CFUUIDRef                 factoryID;
    UInt32                    refCount;
} MetadataImporterPluginType;

MetadataImporterPluginType  *AllocMetadataImporterPluginType(CFUUIDRef inFactoryID);
void                      DeallocMetadataImporterPluginType(MetadataImporterPluginType *thisInstance);
HRESULT                   MetadataImporterQueryInterface(void *thisInstance,REFIID iid,LPVOID *ppv);
void                     *MetadataImporterPluginFactory(CFAllocatorRef allocator,CFUUIDRef typeID);
ULONG                     MetadataImporterPluginAddRef(void *thisInstance);
ULONG                     MetadataImporterPluginRelease(void *thisInstance);

static MDImporterInterfaceStruct testInterfaceFtbl = {
    NULL,
    MetadataImporterQueryInterface,
    MetadataImporterPluginAddRef,
    MetadataImporterPluginRelease,
    GetMetadataForFile
};

MetadataImporterPluginType *AllocMetadataImporterPluginType(CFUUIDRef inFactoryID)
{
    MetadataImporterPluginType *theNewInstance;

    theNewInstance = (MetadataImporterPluginType *)malloc(sizeof(MetadataImporterPluginType));
    memset(theNewInstance,0,sizeof(MetadataImporterPluginType));

    theNewInstance->conduitInterface = &testInterfaceFtbl;

    theNewInstance->factoryID = CFRetain(inFactoryID);
    CFPlugInAddInstanceForFactory(inFactoryID);

    theNewInstance->refCount = 1;
    return theNewInstance;
}

void DeallocMetadataImporterPluginType(MetadataImporterPluginType *thisInstance)
{
    CFUUIDRef theFactoryID;

    theFactoryID = thisInstance->factoryID;
    free(thisInstance);
    if (theFactoryID){
        CFPlugInRemoveInstanceForFactory(theFactoryID);
        CFRelease(theFactoryID);
    }
}

HRESULT MetadataImporterQueryInterface(void *thisInstance,REFIID iid,LPVOID *ppv)
{
    CFUUIDRef interfaceID;

    interfaceID = CFUUIDCreateFromUUIDBytes(kCFAllocatorDefault,iid);

    if (CFEqual(interfaceID,kMDImporterInterfaceID)){
        ((MetadataImporterPluginType*)thisInstance)->conduitInterface->AddRef(thisInstance);
        *ppv = thisInstance;
        CFRelease(interfaceID);
        return S_OK;
    }else{
        if (CFEqual(interfaceID,IUnknownUUID)){
            ((MetadataImporterPluginType*)thisInstance )->conduitInterface->AddRef(thisInstance);
            *ppv = thisInstance;
            CFRelease(interfaceID);
            return S_OK;
        }else{
            *ppv = NULL;
            CFRelease(interfaceID);
            return E_NOINTERFACE;
        }
    }
}

ULONG MetadataImporterPluginAddRef(void *thisInstance)
{
    ((MetadataImporterPluginType *)thisInstance )->refCount += 1;
    return ((MetadataImporterPluginType*) thisInstance)->refCount;
}

ULONG MetadataImporterPluginRelease(void *thisInstance)
{
    ((MetadataImporterPluginType*)thisInstance)->refCount -= 1;
    if (((MetadataImporterPluginType*)thisInstance)->refCount == 0){
        DeallocMetadataImporterPluginType((MetadataImporterPluginType*)thisInstance );
        return 0;
    }else{
        return ((MetadataImporterPluginType*) thisInstance )->refCount;
    }
}

void *MetadataImporterPluginFactory(CFAllocatorRef allocator,CFUUIDRef typeID)
{
    MetadataImporterPluginType *result;
    CFUUIDRef                 uuid;

    if (CFEqual(typeID,kMDImporterTypeID)){
        uuid = CFUUIDCreateFromString(kCFAllocatorDefault,CFSTR(PLUGIN_ID));
        result = AllocMetadataImporterPluginType(uuid);
        CFRelease(uuid);
        return result;
    }
    return NULL;
}

