import ctypes
import os

def create_payload_file(file_type, metadata=""):
    file_path = f"app/payloads/payload.{file_type}"
    with open(file_path, "wb") as f:
        if file_type == "php":
            f.write(b"<?php echo 'Injected'; ?>")
        elif file_type == "pdf":
            f.write(b"%PDF-1.4\n% Payload PDF\n")
        elif file_type == "jpg":
            f.write(b"\xFF\xD8\xFF Payload JPG")

    if metadata:
        c_lib = ctypes.CDLL("../c_lib/metadata_injector.so")
        c_lib.add_metadata(file_path.encode("utf-8"), metadata.encode("utf-8"))

    return file_path
