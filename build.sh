set -e
#Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${GREEN}[BUILD]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

mkdir -p output


for plugin_name in logger uppercaser rotator flipper typewriter; do
    print_status "Building $plugin_name"
    gcc -fPIC -shared -o output/$plugin_name.so plugins/$plugin_name.c plugins/plugin_common.c  plugins/sync/monitor.c plugins/sync/consumer_producer.c \
    -ldl -lpthread || {
        print_error "Failed to build $plugin_name"
        exit 1
    }
done
gcc main.c plugins/plugin_common.c plugins/sync/consumer_producer.c plugins/sync/monitor.c -o output/analyzer
