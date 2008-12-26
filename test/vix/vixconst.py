"""
Definitions for all the constants in the VIX library.
"""
VIX_API_VERSION                                    = 5

VIX_SERVICEPROVIDER_DEFAULT                        = 1
VIX_SERVICEPROVIDER_VMWARE_SERVER                  = 2
VIX_SERVICEPROVIDER_VMWARE_WORKSTATION             = 3

VIX_INVALID_HANDLE                                 = 0

VIX_PROPERTY_NONE                                  = 0
VIX_PROPERTY_META_DATA_CONTAINER                   = 2
VIX_PROPERTY_HOST_HOSTTYPE                         = 50
VIX_PROPERTY_HOST_API_VERSION                      = 51
VIX_PROPERTY_VM_NUM_VCPUS                          = 101
VIX_PROPERTY_VM_VMX_PATHNAME                       = 103
VIX_PROPERTY_VM_VMTEAM_PATHNAME                    = 105
VIX_PROPERTY_VM_MEMORY_SIZE                        = 106
VIX_PROPERTY_VM_READ_ONLY                          = 107
VIX_PROPERTY_VM_IN_VMTEAM                          = 128
VIX_PROPERTY_VM_POWER_STATE                        = 129
VIX_PROPERTY_VM_TOOLS_STATE                        = 152
VIX_PROPERTY_VM_IS_RUNNING                         = 196
VIX_PROPERTY_VM_SUPPORTED_FEATURES                 = 197
VIX_PROPERTY_JOB_RESULT_ERROR_CODE                 = 3000
VIX_PROPERTY_JOB_RESULT_VM_IN_GROUP                = 3001
VIX_PROPERTY_JOB_RESULT_USER_MESSAGE               = 3002
VIX_PROPERTY_JOB_RESULT_EXIT_CODE                  = 3004
VIX_PROPERTY_JOB_RESULT_COMMAND_OUTPUT             = 3005
VIX_PROPERTY_JOB_RESULT_HANDLE                     = 3010
VIX_PROPERTY_JOB_RESULT_GUEST_OBJECT_EXISTS        = 3011
VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_ELAPSED_TIME = 3017
VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_EXIT_CODE    = 3018
VIX_PROPERTY_JOB_RESULT_ITEM_NAME                  = 3035
VIX_PROPERTY_JOB_RESULT_FOUND_ITEM_DESCRIPTION     = 3036
VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_COUNT        = 3046
VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_HOST         = 3048
VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_FLAGS        = 3049
VIX_PROPERTY_JOB_RESULT_PROCESS_ID                 = 3051
VIX_PROPERTY_JOB_RESULT_PROCESS_OWNER              = 3052
VIX_PROPERTY_JOB_RESULT_PROCESS_COMMAND            = 3053
VIX_PROPERTY_JOB_RESULT_FILE_FLAGS                 = 3054
VIX_PROPERTY_JOB_RESULT_PROCESS_START_TIME         = 3055
VIX_PROPERTY_JOB_RESULT_VM_VARIABLE_STRING         = 3056
VIX_PROPERTY_JOB_RESULT_PROCESS_BEING_DEBUGGED     = 3057
VIX_PROPERTY_FOUND_ITEM_LOCATION                   = 4010
VIX_PROPERTY_SNAPSHOT_DISPLAYNAME                  = 4200
VIX_PROPERTY_SNAPSHOT_DESCRIPTION                  = 4201
VIX_PROPERTY_SNAPSHOT_POWERSTATE                   = 4205

VIX_VMPOWEROP_NORMAL                               = 0
VIX_VMPOWEROP_SUPPRESS_SNAPSHOT_POWERON            = 0x0080
VIX_VMPOWEROP_LAUNCH_GUI                           = 0x0200

VIX_OK                                       = 0
VIX_E_FAIL                                   = 1
VIX_E_OUT_OF_MEMORY                          = 2
VIX_E_INVALID_ARG                            = 3
VIX_E_FILE_NOT_FOUND                         = 4
VIX_E_OBJECT_IS_BUSY                         = 5
VIX_E_NOT_SUPPORTED                          = 6
VIX_E_FILE_ERROR                             = 7
VIX_E_DISK_FULL                              = 8
VIX_E_INCORRECT_FILE_TYPE                    = 9
VIX_E_CANCELLED                              = 10
VIX_E_FILE_READ_ONLY                         = 11
VIX_E_FILE_ALREADY_EXISTS                    = 12
VIX_E_FILE_ACCESS_ERROR                      = 13
VIX_E_REQUIRES_LARGE_FILES                   = 14
VIX_E_FILE_ALREADY_LOCKED                    = 15
VIX_E_NOT_SUPPORTED_ON_REMOTE_OBJECT         = 20
VIX_E_FILE_TOO_BIG                           = 21
VIX_E_FILE_NAME_INVALID                      = 22
VIX_E_ALREADY_EXISTS                         = 23

VIX_E_INVALID_HANDLE                         = 1000
VIX_E_NOT_SUPPORTED_ON_HANDLE_TYPE           = 1001
VIX_E_TOO_MANY_HANDLES                       = 1002

VIX_E_NOT_FOUND                              = 2000
VIX_E_TYPE_MISMATCH                          = 2001
VIX_E_INVALID_XML                            = 2002

VIX_E_TIMEOUT_WAITING_FOR_TOOLS              = 3000
VIX_E_UNRECOGNIZED_COMMAND                   = 3001
VIX_E_OP_NOT_SUPPORTED_ON_GUEST              = 3003
VIX_E_PROGRAM_NOT_STARTED                    = 3004
VIX_E_CANNOT_START_READ_ONLY_VM              = 3005
VIX_E_VM_NOT_RUNNING                         = 3006
VIX_E_VM_IS_RUNNING                          = 3007
VIX_E_CANNOT_CONNECT_TO_VM                   = 3008
VIX_E_POWEROP_SCRIPTS_NOT_AVAILABLE          = 3009
VIX_E_NO_GUEST_OS_INSTALLED                  = 3010
VIX_E_VM_INSUFFICIENT_HOST_MEMORY            = 3011
VIX_E_SUSPEND_ERROR                          = 3012
VIX_E_VM_NOT_ENOUGH_CPUS                     = 3013
VIX_E_HOST_USER_PERMISSIONS                  = 3014
VIX_E_GUEST_USER_PERMISSIONS                 = 3015
VIX_E_TOOLS_NOT_RUNNING                      = 3016
VIX_E_GUEST_OPERATIONS_PROHIBITED            = 3017
VIX_E_ANON_GUEST_OPERATIONS_PROHIBITED       = 3018
VIX_E_ROOT_GUEST_OPERATIONS_PROHIBITED       = 3019
VIX_E_MISSING_ANON_GUEST_ACCOUNT             = 3023
VIX_E_CANNOT_AUTHENTICATE_WITH_GUEST         = 3024
VIX_E_UNRECOGNIZED_COMMAND_IN_GUEST          = 3025
VIX_E_CONSOLE_GUEST_OPERATIONS_PROHIBITED    = 3026
VIX_E_MUST_BE_CONSOLE_USER                   = 3027
VIX_E_VMX_MSG_DIALOG_AND_NO_UI               = 3028
VIX_E_OPERATION_NOT_ALLOWED_FOR_LOGIN_TYPE   = 3031
VIX_E_LOGIN_TYPE_NOT_SUPPORTED               = 3032

VIX_E_VM_NOT_FOUND                           = 4000
VIX_E_NOT_SUPPORTED_FOR_VM_VERSION           = 4001
VIX_E_CANNOT_READ_VM_CONFIG                  = 4002
VIX_E_TEMPLATE_VM                            = 4003
VIX_E_VM_ALREADY_LOADED                      = 4004
VIX_E_VM_ALREADY_UP_TO_DATE                  = 4006

VIX_E_UNRECOGNIZED_PROPERTY                  = 6000
VIX_E_INVALID_PROPERTY_VALUE                 = 6001
VIX_E_READ_ONLY_PROPERTY                     = 6002
VIX_E_MISSING_REQUIRED_PROPERTY              = 6003

VIX_E_BAD_VM_INDEX                           = 8000

VIX_E_SNAPSHOT_INVAL                         = 13000
VIX_E_SNAPSHOT_DUMPER                        = 13001
VIX_E_SNAPSHOT_DISKLIB                       = 13002
VIX_E_SNAPSHOT_NOTFOUND                      = 13003
VIX_E_SNAPSHOT_EXISTS                        = 13004
VIX_E_SNAPSHOT_VERSION                       = 13005
VIX_E_SNAPSHOT_NOPERM                        = 13006
VIX_E_SNAPSHOT_CONFIG                        = 13007
VIX_E_SNAPSHOT_NOCHANGE                      = 13008
VIX_E_SNAPSHOT_CHECKPOINT                    = 13009
VIX_E_SNAPSHOT_LOCKED                        = 13010
VIX_E_SNAPSHOT_INCONSISTENT                  = 13011
VIX_E_SNAPSHOT_NAMETOOLONG                   = 13012
VIX_E_SNAPSHOT_VIXFILE                       = 13013
VIX_E_SNAPSHOT_DISKLOCKED                    = 13014
VIX_E_SNAPSHOT_DUPLICATEDDISK                = 13015
VIX_E_SNAPSHOT_INDEPENDENTDISK               = 13016
VIX_E_SNAPSHOT_NONUNIQUE_NAME                = 13017

VIX_E_NOT_A_FILE                             = 20001
VIX_E_NOT_A_DIRECTORY                        = 20002
VIX_E_NO_SUCH_PROCESS                        = 20003
VIX_E_FILE_NAME_TOO_LONG                     = 20004

VIX_RUNPROGRAM_RETURN_IMMEDIATELY   = 0x0001
VIX_RUNPROGRAM_ACTIVATE_WINDOW      = 0x0002

VIX_SHAREDFOLDER_WRITE_ACCESS     = 0x04
