export const classify = asm => {
    if (!asm) return "invalid";
    const first = asm.split(" ")[0];
    if (first.startsWith("f")) return "float";
    if (first.startsWith("c.")) return "compressed";
    return first;
  };
  