import math

tables = [
    { "name": "cos", "fn": math.cos },
    { "name": "sin", "fn": math.sin },
#    { "name": "tan", "fn": math.tan },
]


#print("class Tables {")

#for item in tables:
#    print(f"  private static final int[] {item['name']} = new int[] {'{'}")
#    for i in range(0, 256):
#        x = i / 128 * math.pi
#        val = int(item['fn'](x)*256)
#        if i == 255:
#            print(f"    {val}")
#        else:
#            print(f"    {val},")
#    print("  };")

#    print(f"  public static final int {item['name']}(int x) {'{'}")
#    print(f"    return {item['name']}[x&255];")
#    print("  }")

#print("}")

print("#include \"lut.h\"")

for item in tables:
    print(f"const int {item['name']}_lut[256] = {'{'}")
    for i in range(0, 256):
        x = i / 128.0 * math.pi
        val = int(item['fn'](x)*256)
        if i == 255:
            print(f"  {val}")
        else:
            print(f"  {val},")
    print("};")

    print(f"int {item['name']}(int x)")
    print("{")
    print(f"  return {item['name']}_lut[x&255];")
    print("}")
