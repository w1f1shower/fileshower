import ctypes
import os

# Load the shared library
lib_path = os.path.join(os.path.dirname(__file__), "../../c_lib/metadata_injector.so")
metadata_injector = ctypes.CDLL(lib_path)

# Define the argument and return types for the C functions
metadata_injector.add_metadata.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
metadata_injector.add_metadata.restype = ctypes.c_char_p

def add_metadata(file_path: str, metadata: str):


    try:
        # Convert Python strings to C-compatible byte strings
        file_path_c = file_path.encode("utf-8")
        metadata_c = metadata.encode("utf-8")

        # Call the C function
        metadata_injector.add_metadata(file_path_c, metadata_c)
        print(f"Metadata successfully injected into {file_path_c}.")
        return file_path_c
    except Exception as e:
        print(f"Error injecting metadata into {file_path_c}: {str(e)}")
        raise
