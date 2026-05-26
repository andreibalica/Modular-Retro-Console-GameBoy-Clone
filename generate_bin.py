Import("env")
import subprocess, os

def make_bin(source, target, env):
	elf      = os.path.join(env.subst("$BUILD_DIR"), "firmware.elf")
	env_name = os.path.basename(env.subst("$BUILD_DIR").rstrip("/\\"))
	out_dir  = os.path.join(env.subst("$PROJECT_DIR"), "games", env_name)
	os.makedirs(out_dir, exist_ok=True)
	out      = os.path.join(out_dir, "FIRMWARE.BIN")
	objcopy  = env.subst("$OBJCOPY")
	subprocess.run([objcopy, "-O", "binary", elf, out], check=True)
	print(f"\n>>> FIRMWARE.BIN generat in: {out_dir}")
	print(f">>> Copiaza FIRMWARE.BIN de acolo direct pe cardul SD\n")

env.AddPostAction("$BUILD_DIR/firmware.hex", make_bin)